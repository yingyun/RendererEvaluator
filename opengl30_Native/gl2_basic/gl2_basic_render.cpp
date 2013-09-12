/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of GLES render.
*Detailed see class header.
*
*
*/

#include <pthread.h>
#include "gl2_basic_render.h"


namespace android
{

/* ---------- Data Definition Area Start ---------- */

GLclampf gl2_basic_render::gColorMatrix[6][4] =
{
    {1.0, 1.0, 1.0, 1.0},	//White
    {1.0, 0.0, 0.0, 1.0},	//Red
    {0.0, 1.0, 0.0, 1.0},	//Green
    {1.0, 1.0, 0.0, 1.0},	//Yellow
    {0.0, 0.0, 1.0, 1.0},	//Blue
    {1.0, 0.0, 1.0, 1.0}	//Magenta
};

//simple triangle vertices.
GLfloat gl2_basic_render::gSimpleTriangleVertices[6] =
{ 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

/*
* Cui.YY
* These constructor idea get from Android HWUI design
*/

/*
*   FixMe; Notice while add render command control
*
*   Below source will be add/remvoe in the runtume, so in order to
*   correct frame drawing, add the related logic in three part.
*   1: polygonShaderSetup
*   2: polygonBuildnLink
*   3: poligonDraw
*
*/

//Shader for vertex
const char * gl2_basic_render::gVS_Header_Attribute_vPosition =
    "attribute vec4 vPosition;\n";
const char * gl2_basic_render::gVS_Main_Start_Function =
    "void main() {\n";
const char * gl2_basic_render::gVS_Function_Direct_Pass_Position =
    "   gl_Position = vPosition;\n";
const char * gl2_basic_render::gVS_Main_End_Function =
    "}\n";


//Shader for fragment
const char * gl2_basic_render::gFS_Header_Precision_Mediump_Float =
    "precision mediump float;\n";
const char * gl2_basic_render::gFS_Main_Start_Function =
    "void main() {\n";
const char * gl2_basic_render::gFS_Function_Direct_Pass_Color =
    "  gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n";
const char * gl2_basic_render::gFS_Main_End_Function =
    "}\n"; 

/* ---------- Data Definition Area End ---------- */



//TODO:
// simple quad vertices

// complex vertices

gl2_basic_render::gl2_basic_render(unsigned int index, unsigned int step)
:mAttrVSPosition(0),mIndex(index), mStep(step), mCounter(1), mOGLProgram(0),  mOldTimeStamp(0)
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


void gl2_basic_render::polygonShaderSetup()
{
    //Setup Vertext shader
    mVertexShader.append(gVS_Header_Attribute_vPosition);
    mVertexShader.append(gVS_Main_Start_Function);
    mVertexShader.append(gVS_Function_Direct_Pass_Position);  //FixMe; need to add condition control
    mVertexShader.append(gVS_Main_End_Function);
    printf("VertextShader; \n");
    printf("%s\n", mVertexShader.string());

    //Setup Fragment shader
    mFramgmentShader.append(gFS_Header_Precision_Mediump_Float);
    mFramgmentShader.append(gFS_Main_Start_Function);
    mFramgmentShader.append(gFS_Function_Direct_Pass_Color);  //FixMe; need to add condition control
    mFramgmentShader.append(gFS_Main_End_Function);
    printf("FragmentShader; \n");
    printf("%s\n", mFramgmentShader.string());
}


bool gl2_basic_render::polygonBuildnLink(int w, int h, const char vertexShader[], const char fragmentShader[])
{
    printf("TID:%d simpleTriangleSetup-Done\n", gettid());

    mOGLProgram= createProgram(vertexShader, fragmentShader);
    if (!mOGLProgram)
        {
            return false;
        }
    mAttrVSPosition= glGetAttribLocation(mOGLProgram, "vPosition");  //FixMe; need to add condition control

    glViewport(0, 0, w, h);
    glClearColor(gColorMatrix[mIndex][0], gColorMatrix[mIndex][1],
                 gColorMatrix[mIndex][2], gColorMatrix[mIndex][3]);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glUseProgram(mOGLProgram);
    glVertexAttribPointer(mAttrVSPosition, 2, GL_FLOAT, GL_FALSE, 0, gSimpleTriangleVertices);
    glEnableVertexAttribArray(mAttrVSPosition);

    return true;
}

void gl2_basic_render::polygonDraw()
{
    glVertexAttribPointer(mAttrVSPosition, 2, GL_FLOAT, GL_FALSE, 0, gSimpleTriangleVertices);
    glEnableVertexAttribArray(mAttrVSPosition);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(mEGLDisplay, mEGLSurface);
}

/*
*   Note: This is Static member function
*/
void gl2_basic_render::frameControl(int fd, int events, void* data)
{
    gl2_basic_render* thisObject = (gl2_basic_render*)data;
    ssize_t num_event;
    DisplayEventReceiver::Event buffer[1];
    num_event = thisObject->mDisplayEventReceiver.getEvents(buffer, 1);

    if(thisObject->mCounter == thisObject->mStep)
        {
            if(num_event > 0)
                {
                    if(buffer[0].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC)
                        {
                            // Real rendering block
                            thisObject->polygonDraw();

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

    if (thisObject->mOldTimeStamp)
        {
            float t = float(buffer[0].header.timestamp - thisObject->mOldTimeStamp) / s2ns(1);
            printf("TID:%d %f ms (%f Hz)--", gettid(), t*1000, 1.0/t);
        }
    thisObject->mOldTimeStamp = buffer[0].header.timestamp;
    printf("TID:%d Event vsync:count=%d\n",gettid(), buffer[0].vsync.count);
}

/*
*   Note: This is Static member function
*/
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
    EGLint w, h;
    thisObject->mEGLDisplay= eglGetDisplay(EGL_DEFAULT_DISPLAY);
    returnValue = eglInitialize(thisObject->mEGLDisplay, &majorVersion, &minorVersion);
    eglChooseConfig(thisObject->mEGLDisplay, s_configAttribs, &myConfig, 1, &numConfigs);
    thisObject-> mEGLSurface= eglCreateWindowSurface(thisObject->mEGLDisplay, myConfig, thisObject->windowSurface, NULL);
    context = eglCreateContext(thisObject->mEGLDisplay, myConfig, EGL_NO_CONTEXT, context_attribs);
    returnValue = eglMakeCurrent(thisObject->mEGLDisplay, thisObject->mEGLSurface, thisObject->mEGLSurface, context);
    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_WIDTH, &w);
    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_HEIGHT, &h);
    printf("TID:%d Window dimensions: %d x %d\n", gettid(), w, h);


    /*
    *   1: Generating polygon shader program
    *   2: Build shader and link it
    *   3: Pass control over to Looper to draw each frame
    */
    thisObject->polygonShaderSetup();

    if(!thisObject->polygonBuildnLink(w, h, 
        thisObject->mVertexShader.string(), thisObject->mFramgmentShader.string()))
        {
            fprintf(stderr, "Could not set up graphics.\n");
            return (void *)0;
        }

    thisObject->mLoop = new Looper(false);
    thisObject->mLoop->addFd(thisObject->mDisplayEventReceiver.getFd(), 0, ALOOPER_EVENT_INPUT,
                             (ALooper_callbackFunc)gl2_basic_render::frameControl, thisObject);
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
    while(true);

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