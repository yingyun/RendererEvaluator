/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of GLES render.
*Detailed see class header.
*
*/
// TODO: add below operation in the shader
/*
* =>1: Add error check for gl function
* =>2: Add lighting
* =>3: Add various buffer object to improve performance
*          In OpenGLES 2.0 we have FBO, VBO
*Cui.YY: The buffer means the buffer on the Chip but it's means in GPU buffer
*on the OpenGL concept.Actually, it's depends on OpenGL driver design.
*Whether desktop or embedded it's all have the embedded memory on
*the GPU IP to store/load the data to improve/reduce performance/mem bandwitdh.
*
* =>4: Add texture mappting
* =>5: Add color                                                                            -Done
* =>6: Depth test, How to handle Z-Fighting
*
*
*
*/

#include <pthread.h>
#include "Main_Renderer.h"

//#define LOG_ENABLE
#ifdef LOG_ENABLE
#define LOG(...) printf(__VA_ARGS__)  //FixMe;  What does VA_ARGS mean?
#else
#define LOG(...)
#endif

namespace android
{
/* ---------- Data Definition Area Start ---------- */
GLclampf gl2_basic_render::gColorMatrix[6][4] =
{
    {0.0, 0.0, 0.0, 0.0},       //Black
    {1.0, 0.0, 0.0, 1.0},	//Red
    {0.0, 1.0, 0.0, 1.0},	//Green
    {1.0, 1.0, 0.0, 1.0},	//Yellow
    {0.0, 0.0, 1.0, 1.0},	//Blue
    {1.0, 1.0, 1.0, 1.0}        //White
};

/* simple triangle vertices.    FixMe; Change to Regular Triangle */
GLfloat gl2_basic_render::gSimpleTriangleVertices[6] =
{-0.5f, -0.288675f,    0.0f, 0.57735f,    0.5f, -0.288675f};

/*
* Cui.YY
* These constructor idea get from Android HWUI design
* Some case will result bad performance of shader as not good-desging
* shader compiler.
*
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
const char * gl2_basic_render::gVS_Header_Attribute_vertexPosition =
    "attribute vec4 vertexPosition;\n";
const char * gl2_basic_render::gVS_Header_Uniform_rotationMatrix =
    "uniform mat4 rotationMatrix;\n";
const char * gl2_basic_render::gVS_Header_Uniform_scaleMatrix =
    "uniform mat4 scaleMatrix;\n";
const char * gl2_basic_render::gVS_Header_Uniform_translationMatrix =
    "uniform mat4 translationMatrix;\n";
const char * gl2_basic_render::gVS_Header_Attribute_passColor =
    "attribute vec4 passColor;\n";
const char * gl2_basic_render::gVS_Header_Varying_colorToFrag =
    "varying vec4 colorToFrag;\n";

const char * gl2_basic_render::gVS_Main_Start_Function =
    "void main() {\n";

const char * gl2_basic_render::gVS_Function_Direct_Pass_Position =
    "   gl_Position = vertexPosition;\n";
const char * gl2_basic_render::gVS_Function_Pass_RO_Multi_Position =
    "   gl_Position = gl_Position * rotationMatrix;\n";
const char * gl2_basic_render::gVS_Function_Pass_SC_Multi_Position =
    "   gl_Position = gl_Position * scaleMatrix;\n";
const char * gl2_basic_render::gVS_Function_Pass_TR_Multi_Position =
    "   gl_Position = gl_Position * translationMatrix;\n";
const char * gl2_basic_render::gVS_Function_Pass_Color_To_Frag =
    "   colorToFrag = passColor;\n";

const char * gl2_basic_render::gVS_Main_End_Function =
    "}\n";

//Shader for fragment
const char * gl2_basic_render::gFS_Header_Precision_Mediump_Float =
    "precision mediump float;\n";
const char * gl2_basic_render::gFS_Header_Varying_colorToFrag =
    "varying vec4 colorToFrag;\n";

const char * gl2_basic_render::gFS_Main_Start_Function =
    "void main() {\n";

const char * gl2_basic_render::gFS_Function_Pass_Constant_Color =
    "  gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n";
const char * gl2_basic_render::gFS_Function_Direct_Pass_Color =
    "  gl_FragColor = colorToFrag;\n";

const char * gl2_basic_render::gFS_Main_End_Function =
    "}\n";
/* ==========Data Definition Area End ==========*/

gl2_basic_render::gl2_basic_render(unsigned int index, unsigned int step)
    :mAttrVSPosition(0),mIndex(index), mStep(step), mCounter(1), mOGLProgram(0),  mOldTimeStamp(0)
{
    /* model view tranformation */
    hasRotation = true; //Done
    hasScale = false;  //Done
    hasTranslation = false;

    /* advanced vertex operation */
    hasLighting = false;
    hasTextureMap = false;
    hasFBO = false;
    hasVBO = true;
    hasVAO = false;

    /* fragment shader */
    hasPreciMidium = true;
    hasColorConstantPass = false;
    hasColorDirectPass = true;

    /* polygon setup */
    hasSimTriangle = false;
    hasCube = true;
    hasBlender = false;

    /* transformation specific */
    mRotationAngle = 0;
    mScaleMagnitude = 0;
    mScaleIncreasing = true;
    mTranslationMagnitude = 0;

    /* polygon specific */
    mCubeVertices = 0;
    mCubeColor = 0;
    mCubeIndices = 0;
    mCubeNumOfIndex = 0;
    mBlenderVertices = 0;

    printf("-Config Print-\n \
   \t hasColorConstantPass \t%d\n \
   \t hasColorDirectPass \t%d\n \
   \t hasPreciMidium \t%d\n  \
   \t hasRotation \t%d\n  \
   \t hasScale \t%d\n    \
   \t hasTranslation %d\n  \
   \t hasLighting \t%d\n   \
   \t hasTextureMap \t%d\n  \
   \t hasFBO \t%d\n  \
   \t hasVBO \t%d\n  \
   \t hasVAO \t%d\n \
   \t hasSimTriangle %d\n \
   \t hasCube\t%d\n \
   \t hasBlender\t%d\n",
           hasColorConstantPass,
           hasColorDirectPass,
           hasPreciMidium,
           hasRotation,
           hasScale,
           hasTranslation,
           hasLighting,
           hasTextureMap,
           hasFBO,
           hasVBO,
           hasVAO,
           hasSimTriangle,
           hasCube,
           hasBlender);
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
    mVertexShader.append(gVS_Header_Attribute_vertexPosition);  //We always pass the vertex to the shader
    if(hasRotation) mVertexShader.append(gVS_Header_Uniform_rotationMatrix);
    if(hasScale) mVertexShader.append(gVS_Header_Uniform_scaleMatrix);
    if(hasTranslation) mVertexShader.append(gVS_Header_Uniform_translationMatrix);
    if(hasColorDirectPass) mVertexShader.append(gVS_Header_Attribute_passColor);
    if(hasColorDirectPass) mVertexShader.append(gVS_Header_Varying_colorToFrag);


    mVertexShader.append(gVS_Main_Start_Function);

    mVertexShader.append(gVS_Function_Direct_Pass_Position);
    if(hasRotation) mVertexShader.append(gVS_Function_Pass_RO_Multi_Position);
    if(hasScale) mVertexShader.append(gVS_Function_Pass_SC_Multi_Position);
    if(hasTranslation) mVertexShader.append(gVS_Function_Pass_TR_Multi_Position);
    if(hasColorDirectPass) mVertexShader.append(gVS_Function_Pass_Color_To_Frag);

    mVertexShader.append(gVS_Main_End_Function);
    printf("VertextShader; \n");
    printf("%s\n", mVertexShader.string());

    //Setup Fragment shader
    if(hasPreciMidium)mFramgmentShader.append(gFS_Header_Precision_Mediump_Float);
    if(hasColorDirectPass)mFramgmentShader.append(gFS_Header_Varying_colorToFrag);

    mFramgmentShader.append(gFS_Main_Start_Function);

    if(hasColorConstantPass)mFramgmentShader.append(gFS_Function_Pass_Constant_Color);
    if(hasColorDirectPass)mFramgmentShader.append(gFS_Function_Direct_Pass_Color);

    mFramgmentShader.append(gFS_Main_End_Function);
    printf("FragmentShader; \n");
    printf("%s\n", mFramgmentShader.string());

    /* Do polygon vertex generation */
    //generateCube(bool indexMode, float scale, float * * vertices, float * * normals, float * * texCoords, float * * colors, unsigned int * * indices)
    if(hasCube)mCubeNumOfIndex = VertexGenerator::generateCube(true, 0.5, &mCubeVertices,
                                     NULL, NULL, &mCubeColor, &mCubeIndices);
}

bool gl2_basic_render::polygonBuildnLink(int w, int h, const char vertexShader[], const char fragmentShader[])
{
    mOGLProgram= createProgram(vertexShader, fragmentShader);
    if (!mOGLProgram)
        {
            return false;
        }
    //Retrieve shader paramer information
    mAttrVSPosition = glGetAttribLocation(mOGLProgram, "vertexPosition");

    if(hasRotation)
        mUniVSrotateMat = glGetUniformLocation(mOGLProgram, "rotationMatrix");
    if(hasScale)
        mUniVSscaleMat = glGetUniformLocation(mOGLProgram, "scaleMatrix");
    if(hasTranslation)
        mUniVStranslateMat = glGetUniformLocation(mOGLProgram, "translationMatrix");
    if(hasColorDirectPass)
        mAttrVSColorPass = glGetAttribLocation(mOGLProgram, "passColor");


    //Create various buffer object
    if(hasCube && hasVBO)
        {
            glGenBuffers(3, mVBOForVI); //Note: This is just used for Index mode rendering
            //[0] for Vertex
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[0]);
            glBufferData(GL_ARRAY_BUFFER, VertexGenerator::vertexSizeByte(true),
                         mCubeVertices, GL_STATIC_DRAW);

            //[1] for color
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[1]);
            glBufferData(GL_ARRAY_BUFFER, VertexGenerator::colorSizeByte(true),
                mCubeColor, GL_STATIC_DRAW);

            //[2] for index
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOForVI[2]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 500/*VertexGenerator::indexSizeByte()*/,
                         mCubeIndices, GL_STATIC_DRAW);
           }

    glViewport(0, 0, w, h);
    glClearColor(gColorMatrix[mIndex][0], gColorMatrix[mIndex][1],
                 gColorMatrix[mIndex][2], gColorMatrix[mIndex][3]);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Culling should always be enabled to improve the perf of OpenGL, Culling back and CCW was defualt config
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glUseProgram(mOGLProgram);

    return true;
}

void gl2_basic_render::polygonDraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    /* polygon vertex data */
    if(hasSimTriangle)glVertexAttribPointer(mAttrVSPosition, 2, GL_FLOAT, GL_FALSE, 0, gSimpleTriangleVertices);
    if(hasCube && hasVBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[0]);
            glVertexAttribPointer(mAttrVSPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(mAttrVSPosition);
        }
    else if(hasCube)
        {
            glVertexAttribPointer(mAttrVSPosition, 3, GL_FLOAT, GL_FALSE, 0, mCubeVertices);
            glEnableVertexAttribArray(mAttrVSPosition);
        }
    //    glEnableVertexAttribArray(mAttrVSPosition);  //FixMe; Maybe Change?

    /* do transformantion */
    if(hasRotation)
        {
            MatrixTransform::matrixIndentity(&mRotateMatrix);
            /*
            * Cui.YY 20130916
            * Why (++mRotationAngle) % 360 didn't pass the compiler as it's called undefined behaviour.
            * The compiler can do re-order feel free, so, cannot ensure the same result in different compiler
            * even with same compiler with different settings.
            * So, in C/C++ we can just modify one value in single line, can't modify same value in both
            * operands.  http://stackoverflow.com/questions/9951993/why-foo-foo-may-be-undefined
            */
            ++mRotationAngle;
            mRotationAngle = mRotationAngle % 360;//The step of angle increasing
            MatrixTransform::matrixRotate(&mRotateMatrix, (GLfloat)mRotationAngle, 1.0, 1.0, 0.0);
            MatrixTransform::matrixDump(&mRotateMatrix, "mRotateMatrix");
            glUniformMatrix4fv(mUniVSrotateMat, 1, GL_FALSE, (GLfloat * )mRotateMatrix.m);
        }
    if(hasScale)
        {
            /* do repeast operation for the change of scale magnitude */
            if (mScaleIncreasing)
                {

                    mScaleMagnitude += 0.01;
                    if (mScaleMagnitude > (GLfloat)1.5)
                        {
                            mScaleIncreasing = false;
                        }
                }
            else
                {
                    mScaleMagnitude -= 0.01;
                    if (mScaleMagnitude < (GLfloat)0.0)
                        {
                            mScaleIncreasing = 1;
                        }
                }
            MatrixTransform::matrixIndentity(&mScaleMatrix);
            MatrixTransform::matrixScale(&mScaleMatrix, mScaleMagnitude, mScaleMagnitude, mScaleMagnitude);
            MatrixTransform::matrixDump(&mScaleMatrix, "mScaleMatrix");
            glUniformMatrix4fv(mUniVSscaleMat, 1, GL_FALSE, (GLfloat * )mScaleMatrix.m);
        }
    if(hasTranslation)
        {
            mTranslationMagnitude += 0.1;
            if (mTranslationMagnitude > 1.0f) mTranslationMagnitude = 0;
            MatrixTransform::matrixIndentity(&mTranslateMatrix);
            /* FixMe; matrixTranslate was wrong ?  */
            MatrixTransform::matrixTranslate(&mTranslateMatrix, 0.0f, 0.0f, 0.0f);
            MatrixTransform::matrixDump(&mTranslateMatrix, "mTranslateMatrix");
            glUniformMatrix4fv(mUniVStranslateMat, 1, GL_FALSE, (GLfloat * )mTranslateMatrix.m);
        }
    /* Color and Light */
    if(hasColorDirectPass && hasVBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[1]);
            glVertexAttribPointer(mAttrVSColorPass, 4, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(mAttrVSColorPass);

        }
    else if (hasColorDirectPass)
        {
            glVertexAttribPointer(mAttrVSColorPass, 4, GL_FLOAT, GL_FALSE, 0, mCubeColor);
            glEnableVertexAttribArray(mAttrVSColorPass);
        }

    /* Let's cook */
    if(hasSimTriangle)glDrawArrays(GL_TRIANGLES, 0, 3);
    if(hasCube && hasVBO)
        glDrawElements(GL_TRIANGLES, mCubeNumOfIndex, GL_UNSIGNED_INT, 0);
    else if(hasCube)
        glDrawElements(GL_TRIANGLES, mCubeNumOfIndex, GL_UNSIGNED_INT, mCubeIndices); //Index mode
    //if(hasCube)glDrawArrays(GL_TRIANGLES, 0, 36);  //No index mode
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

                            LOG("TID:%d swap buffers--", gettid());
                        }
                }
            thisObject->mCounter = 1;
        }
    else
        {
            LOG("TID:%d skip buffers--", gettid());
            thisObject->mCounter++;
        }

    if (thisObject->mOldTimeStamp)
        {
            float t = float(buffer[0].header.timestamp - thisObject->mOldTimeStamp) / s2ns(1);
            LOG("TID:%d %f ms (%f Hz)--", gettid(), t*1000, 1.0/t);
        }
    thisObject->mOldTimeStamp = buffer[0].header.timestamp;
    LOG("TID:%d Event vsync:count=%d\n",gettid(), buffer[0].vsync.count);
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
    unsigned int tid = gettid();

    do
        {
            int ret = thisObject->mLoop->pollOnce(-1);
            switch(ret)
                {
                    case ALOOPER_POLL_WAKE:
                        printf("TID:%d Poll wake\n", tid);
                        break;
                    case ALOOPER_POLL_CALLBACK:
                        /* printf("TID:%d Poll callback\n", tid); */
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

    return 0;
}

void gl2_basic_render::startRender(EGLNativeWindowType window)
{
    pthread_t thread_status;
    windowSurface = window;
    pthread_create(&thread_status, NULL, gl2_basic_render::mainRender, (void*)(this));
}

}
