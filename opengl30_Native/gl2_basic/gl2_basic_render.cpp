/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of gl render.
*Detailed see class header.
*
*
*/

#include <pthread.h>

#include "gl2_basic_render.h"


namespace android
{

GLclampf gl2_basic_render::mColorMatrix[6][4] =
{
    {1.0, 1.0, 1.0, 1.0},	//White
    {1.0, 0.0, 0.0, 1.0},	//Red
    {0.0, 1.0, 0.0, 1.0},	//Green
    {1.0, 1.0, 0.0, 1.0},	//Yellow
    {0.0, 0.0, 1.0, 1.0},	//Blue
    {1.0, 0.0, 1.0, 1.0}	//Magenta
};
gl2_basic_render::gl2_basic_render(unsigned int index, unsigned int step):mIndex(index), mStep(step), mCounter(1)
    ,gProgram(0), gvPositionHandle(0), oldTimeStamp(0)
{
    /*
    *Construction. Nothing to do now
    */
}

GLuint gl2_basic_render::loadShader(GLenum shaderType, const char* pSource)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
                {
                    GLint infoLen = 0;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                    if (infoLen)
                        {
                            char* buf = (char*) malloc(infoLen);
                            if (buf)
                                {
                                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                                    fprintf(stderr, "Could not compile shader %d:\n%s\n",
                                            shaderType, buf);
                                    free(buf);
                                }
                            glDeleteShader(shader);
                            shader = 0;
                        }
                }
        }
    return shader;
}

GLuint gl2_basic_render::createProgram(const char* pVertexSource, const char* pFragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader)
        {
            return 0;
        }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader)
        {
            return 0;
        }

    GLuint program = glCreateProgram();
    if (program)
        {
            glAttachShader(program, vertexShader);
            glAttachShader(program, pixelShader);
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE)
                {
                    GLint bufLength = 0;
                    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                    if (bufLength)
                        {
                            char* buf = (char*) malloc(bufLength);
                            if (buf)
                                {
                                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                                    fprintf(stderr, "Could not link program:\n%s\n", buf);
                                    free(buf);
                                }
                        }
                    glDeleteProgram(program);
                    program = 0;
                }
        }
    return program;
}



bool gl2_basic_render::setupGraphics(int w, int h)
{
    printf("TID:%d setupGraphics-Done\n", gettid());

    /*
    * Shader source sould be imported from outside storage.
    */
    const char gVertexShader[] = "attribute vec4 vPosition;\n"
                                 "void main() {\n"
                                 "  gl_Position = vPosition;\n"
                                 "}\n";

    const char gFragmentShader[] = "precision mediump float;\n"
                                   "void main() {\n"
                                   "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                                   "}\n";

    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram)
        {
            return false;
        }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
//    fprintf(stderr, "glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandle);

    glViewport(0, 0, w, h);
    return true;
}

void gl2_basic_render::renderFrame(int fd, int events, void* data)
{
    gl2_basic_render* thisObject = (gl2_basic_render*)data;
    ssize_t num_event;
    DisplayEventReceiver::Event buffer[1];
    num_event = thisObject->mDisplayEventReceiver.getEvents(buffer, 1);

    if(thisObject->mCounter == thisObject->mStep)
        {
            unsigned int index = thisObject->mIndex;
            if(num_event > 0)
                {
                    if(buffer[0].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC)
                        {
                            const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
                            glClearColor(mColorMatrix[index][0], mColorMatrix[index][1],
                                         mColorMatrix[index][2], mColorMatrix[index][3]);
                            glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
                            glUseProgram(thisObject->gProgram);
                            glVertexAttribPointer(thisObject->gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
                            glEnableVertexAttribArray(thisObject->gvPositionHandle);
                            glDrawArrays(GL_TRIANGLES, 0, 3);
                            eglSwapBuffers(thisObject->dpy, thisObject->surface);
                            printf("TID:%d swap buffers--", gettid());
                        }
                }
            thisObject->mCounter = 1;
        }
    else
        {
            printf("TID:%d skip buffers--", gettid());
            thisObject->mCounter++;
        }

    if (thisObject->oldTimeStamp)
        {
            float t = float(buffer[0].header.timestamp - thisObject->oldTimeStamp) / s2ns(1);
            printf("TID:%d %f ms (%f Hz)--", gettid(), t*1000, 1.0/t);
        }
    thisObject->oldTimeStamp = buffer[0].header.timestamp;
    printf("TID:%d Event vsync:count=%d\n",gettid(), buffer[0].vsync.count);
}

void* gl2_basic_render::mainRender(void* thisthis)
{
    gl2_basic_render * thisObject = (gl2_basic_render*)thisthis;
    printf("TID:%d Create render\n", gettid());
    EGLBoolean returnValue;
    EGLConfig myConfig = {0};
    EGLint numConfigs;

    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLint s_configAttribs[] =
    {
        EGL_SURFACE_TYPE,        EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLint majorVersion;
    EGLint minorVersion;
    EGLContext context;
//    EGLSurface surface;
//    EGLDisplay dpy;
    EGLint w, h;

    thisObject->dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    returnValue = eglInitialize(thisObject->dpy, &majorVersion, &minorVersion);
    eglChooseConfig(thisObject->dpy, s_configAttribs, &myConfig, 1, &numConfigs);

    //EGL native surface be passed by function argument, it's the responsible of
    //Class caller to make sure the window surface was valid.
    thisObject-> surface = eglCreateWindowSurface(thisObject->dpy, myConfig, thisObject->windowSurface, NULL);
    context = eglCreateContext(thisObject->dpy, myConfig, EGL_NO_CONTEXT, context_attribs);
    returnValue = eglMakeCurrent(thisObject->dpy, thisObject->surface, thisObject->surface, context);

    eglQuerySurface(thisObject->dpy, thisObject->surface, EGL_WIDTH, &w);
    eglQuerySurface(thisObject->dpy, thisObject->surface, EGL_HEIGHT, &h);
    printf("TID:%d Window dimensions: %d x %d\n", gettid(), w, h);

    if(!thisObject->setupGraphics(w, h))
        {
            fprintf(stderr, "Could not set up graphics.\n");
            return (void *)0;
        }
    /*    for (;;)
            {
                thisObject->renderFrame();
                eglSwapBuffers(dpy, surface);
            }*/
    thisObject->mLoop = new Looper(false);
    thisObject->mLoop->addFd(thisObject->mDisplayEventReceiver.getFd(), 0, ALOOPER_EVENT_INPUT,
                             (ALooper_callbackFunc)gl2_basic_render::renderFrame, thisObject);
    thisObject->mDisplayEventReceiver.setVsyncRate(1);//Enable vsync forever
    int tid = gettid();
    printf("TID:%d Enable Vsync\n", tid);

    do
        {
            int ret = thisObject->mLoop->pollOnce(-1);
            switch(ret)
                {
                case ALOOPER_POLL_WAKE:
                    printf("TID:%d Poll wake\n", tid);
                    break;
                case ALOOPER_POLL_CALLBACK:
//                    printf("TID:%d Poll callback\n", tid);
                    break;
                case ALOOPER_POLL_TIMEOUT:
                    printf("TID:%d Poll timeout\n", tid);
                    break;
                case ALOOPER_POLL_ERROR:
                    printf("TID:%d Poll error\n", tid);
                    break;
                default:
                    printf("TID:%d What???\n", tid);
                    break;
                }
        }
    while(1);

//    sleep(99999999999);
    return 0;
}

void gl2_basic_render::startRender(EGLNativeWindowType window)
{
    pthread_t thread_status;
    windowSurface = window;
    pthread_create(&thread_status, NULL, gl2_basic_render::mainRender, (void*)(this));
}

}
