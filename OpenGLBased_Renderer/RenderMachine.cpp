/*
*2013-2014 Cui.YY
*/

#include "RenderMachine.h"

namespace RenderEvaluator
{
RenderMachine::RenderMachine(LayerRenderType layerInfo)
{
    mLayerInfo= layerInfo;
}

void RenderMachine::polygonEffectSelect()
{
    /* FixMe; TODO: Add runtime switch to change the effect dynamically. */
    mEffect = new GrayscaleEffect(mLayerInfo);
}

bool RenderMachine::polygonViewSetup()
{
    /* Viewport setup */
    glViewport(0, 0, mLayerInfo.LayerWidth, mLayerInfo.LayerHeight);
    glDepthRangef(0, 1);

    /* Buffer Clear */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepthf(0);
    glClearStencil(0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Post Fragment operation */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    return true;
}

bool RenderMachine::polygonOnceRoutine()
{
    if( !mEffect->updateShaderOnce()) return false;
    if(!mEffect->updateAttributeOnce()) return false;
    if(!mEffect->updateBufferOnce()) return false;
    return true;
}

void RenderMachine::polygonDraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if(!mEffect->updateFrameEvery()) LOG_ERROR("Updating frame every error!\n");
    if(!mEffect->drawPolygonEvery()) LOG_ERROR("Drawing polygon every error!\n");

    eglSwapBuffers(mEGLDisplay, mEGLSurface);
}

void RenderMachine::frameControl(int fd, int events, void* data)
{
    RenderMachine* thisObject = (RenderMachine*)data;
    ssize_t num_event;
    DisplayEventReceiver::Event buffer[1];
    num_event = thisObject->mDisplayEventReceiver.getEvents(buffer, 1);

    if(thisObject->mCounter == thisObject->mLayerInfo.LayerDrawTick)
        {
            if(num_event > 0)
                {
                    if(buffer[0].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC)
                        {
                            thisObject->polygonDraw();
                            LOG_DEBUG("TID:%d swap buffers--", gettid());
                        }
                }
            thisObject->mCounter = 1;
        }
    else
        {
            LOG_DEBUG("TID:%d skip buffers--", gettid());
            thisObject->mCounter++;
        }

    if (thisObject->mOldTimeStamp)
        {
            float t = float(buffer[0].header.timestamp - thisObject->mOldTimeStamp) / s2ns(1);
            LOG_DEBUG("TID:%d %f ms (%f Hz)--", gettid(), t*1000, 1.0/t);
        }
    thisObject->mOldTimeStamp = buffer[0].header.timestamp;
    LOG_DEBUG("TID:%d Event vsync:count=%d\n",gettid(), buffer[0].vsync.count);
}

/* Cui.YY: This is Static member function */
void* RenderMachine::mainRender(void* thisthis)
{
    RenderMachine * thisObject = (RenderMachine*)thisthis;
    LOG_INFO("TID:%d Create render\n", gettid());

    EGLBoolean returnValue;
    EGLConfig myConfig = {0};
    EGLint numConfigs = 0;
    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    /*
    *More detailed refer below link
    *http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglChooseConfig.html
    */
    EGLint s_configAttribs[] =
    {
        EGL_SURFACE_TYPE,        EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,                   8,
        EGL_GREEN_SIZE,               8,
        EGL_BLUE_SIZE,                  8,
        EGL_ALPHA_SIZE,               0,
        EGL_DEPTH_SIZE,               24,
        EGL_STENCIL_SIZE,            8,
        EGL_SAMPLE_BUFFERS, 1, //Enable MSAA
        EGL_SAMPLES, 4,                //MSAA X4
        EGL_NONE
    };
    EGLint majorVersion;
    EGLint minorVersion;
    EGLContext context;

    /* EGL init*/
    thisObject->mEGLDisplay= eglGetDisplay(EGL_DEFAULT_DISPLAY);
    returnValue = eglInitialize(thisObject->mEGLDisplay, &majorVersion, &minorVersion);

    /* EGL Choose our configuration context */
    eglChooseConfig(thisObject->mEGLDisplay, s_configAttribs, &myConfig, 1, &numConfigs);
    LOG_INFO("---We match %d number config(s)-----\n", numConfigs);
    //thisObject->printEGLConfigInformation(myConfig);

    /* EGL Create window surface and Context */
    thisObject-> mEGLSurface= eglCreateWindowSurface(thisObject->mEGLDisplay, myConfig, thisObject->mWindowSurface, NULL);
    context = eglCreateContext(thisObject->mEGLDisplay, myConfig, EGL_NO_CONTEXT, context_attribs);

    /* EGL Make current as available */
    returnValue = eglMakeCurrent(thisObject->mEGLDisplay, thisObject->mEGLSurface, thisObject->mEGLSurface, context);

    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_WIDTH, &(thisObject->surfaceWidth));
    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_HEIGHT, &(thisObject->surfaceHeight));
    LOG_INFO("TID:%d Window dimensions: %d x %d\n", gettid(), thisObject->surfaceWidth, thisObject->surfaceHeight);

    thisObject->polygonEffectSelect();
    thisObject->polygonViewSetup();
    thisObject->polygonOnceRoutine();

    /* Create loop pool to handle event */
    thisObject->mLoop = new Looper(false);
    thisObject->mLoop->addFd(thisObject->mDisplayEventReceiver.getFd(), 0, ALOOPER_EVENT_INPUT,
                             (ALooper_callbackFunc)RenderMachine::frameControl, thisObject);

    thisObject->mDisplayEventReceiver.setVsyncRate(1);//Enable vsync forever

    unsigned int tid = gettid();
    do
        {
            int ret = thisObject->mLoop->pollOnce(-1);
            switch(ret)
                {
                    case ALOOPER_POLL_WAKE:
                        LOG_ERROR("TID:%d Poll wake\n", tid);
                        break;
                    case ALOOPER_POLL_CALLBACK:
                        /* printf("TID:%d Poll callback\n", tid); */
                        break;
                    case ALOOPER_POLL_TIMEOUT:
                        LOG_ERROR("TID:%d Poll timeout\n", tid);
                        break;
                    case ALOOPER_POLL_ERROR:
                        LOG_ERROR("TID:%d Poll error\n", tid);
                        break;
                    default:
                        LOG_ERROR("TID:%d What???\n", tid);
                        break;
                }
        }
    while(true);
    return 0;
}
void RenderMachine::startRender(EGLNativeWindowType surface)
{
    pthread_t thread_status;
    mWindowSurface = surface;
    pthread_create(&thread_status, NULL, RenderMachine::mainRender, (void*)(this));
}

}
