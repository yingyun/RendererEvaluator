/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of GLES render.
*Detailed see class header.
*
* TODO: add below operation in the shader
* =>1: Add error check for gl function
* =>2: Post-processing : Gamma, Contrast, Saturation, Hue and so on
* =>3: Gaussian Blur
* =>4: Objective model import //???
* =>5: various texture mapping
* =>6: Add GPU composing Evaluator, Refer ANDROID/frameworks/native/cmd/flatland
* =>7: Google test framework used for SurfaceTexture testing.
* =>8: Change the strategy for surface size and viewport                                -> Done;
* =>9: Specify the texture file via program argument
* =>10: GrayScale image manipulation;                                                   -> Done;
* =>11: Add config file parse to control various operation for rendering
*/

#include "RenderMachine.h"
//#define LOG_ENABLE
#ifdef LOG_ENABLE
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif
#define error_print(ABC) fprintf(stderr, "ERROR %s +%d:"ABC"\n", __func__, __LINE__)
#define PHY_SCREEN_WIDTH 1080
#define PHY_SCREEN_HIGHT 1800

namespace android
{
/* ---------- Data Definition Area Start ---------- */
GLclampf RenderMachine::gColorMatrix[6][4] =
{
    {0.2, 0.6, 0.4, 1.0},      //DeepGreen
    {0.0, 0.0, 0.0, 1.0},	//Black
    {0.1, 0.6, 0.1, 1.0},	//Green
    {0.8, 0.8, 0.4, 1.0},	//Yellow
    {0.3, 0.2, 0.6, 1.0},	//Blue
    {0.5, 0.5, 0.5, 1.0}       //White
};

/* simple triangle vertices.    FixMe; Change to Regular Triangle */
GLfloat RenderMachine::gSimpleTriangleVertices[6] =
{-0.5f, -0.288675f,    0.0f, 0.57735f,    0.5f, -0.288675f};

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

RenderMachine::RenderMachine(unsigned int index, unsigned int step):
    mIndex(index), mStep(step), mCounter(1), mOGLProgram(0), mOldTimeStamp(0)
{
    /*
    *Cui.YY
    *FixMe; TODO: Below value should be constructed by INI file
    */

    /* model view tranformation */
    hasRotation = false;
    hasScale = false;
    hasTranslation = false;
    hasOrthoProjection = true;
    hasNDCVerticle = false;

    /* Advanced Vertex Operation */
    hasLighting = false;
    hasTexture2D = true;
    hasMipMap = true;
    hasBasicCubeFBO = false;
    hasCubeWithVBO = false;
    hasCubeIndexModeWithoutVBO= false;
    hasColorDirectPassCombimeVBO = false;
    hasVAO = false;

    /* fragment shader */
    hasPreciMidium = true;
    hasColorConstantPass = false;
    hasColorDirectPass = false;

    /* Advanced Fragment Operation */
    hasHuePP = false;
    hasBrightnessPP = false;
    hasContrastPP = false;
    hasSaturationPP = false;
    hasSharpnessPP = false;
    hasGrayscalePP = true;

    /* Various Testing/Evaluate function */
    hasGoogleTest = false;
    hasGPUEvaluation = false;

    /* polygon setup */
    hasSimTriangle = false;
    hasRectangle = true;
    hasCube = false;
    hasBlenderObjectModel = false;
    hasScissor = false;
    hasBasicStencilOpe = false;
    hasBasicDepthTest = false;
    hasBasicBlendingOpe = false;
    hasBasicMSAA = true;
    hasGaussianBlur = false;
    hasCullFace = false;

    /* transformation middle variable init */
    mRotationAngle = 0;
    mScaleMagnitude = 0;
    mScaleIncreasing = true;
    mTranslationMagnitude = 0;

    /* polygon middle variable init */
    mRectangleVertices = 0;
    mCubeVertices = 0;
    mCubeColor = 0;
    mCubeIndices = 0;
    mCubeNumOfIndex = 0;
    mBlenderVertices = 0;

    /* post processing middle variable init */
    mBrightnessMagnitude = 0.0f;

    printRenderingConfiguration();
}

void RenderMachine::printOpenGLDriverInformation()
{
    /*
    * glGetString was used to get the implementation basic information
    */
    char* vender = (char*)glGetString(GL_VENDOR);
    char* version =(char*)glGetString(GL_VERSION);
    char* glrenderer =(char*)glGetString(GL_RENDERER);
    char* glsl_version =(char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    char* extensions =(char*)glGetString(GL_EXTENSIONS);
    printf("OpenGL Vender: %s\n", vender);
    printf("Version: %s\n", version);
    printf("Renderer: %s\n", glrenderer);
    printf("%s\n", glsl_version);
    //    printf("%s\n", extensions);

    int integer4[4] = {0, 0, 0 , 0};
    float float4[4] = {0.0, 0.0, 0.0, 0.0};
    bool bool4[4] = {0, 0, 0, 0};
    printf("-----Current EGL Configuration-----\n");
    glGetIntegerv(GL_VIEWPORT, integer4);
    printf("ViewPort: %d, %d, %d, %d\n", integer4[0], integer4[1], integer4[2], integer4[3]);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, integer4);
    printf("Max ViewPort: %d, %d, %d, %d\n", integer4[0], integer4[1], integer4[2], integer4[3]);
    glGetFloatv(GL_DEPTH_RANGE, float4);
    printf("Depth Range: (%f - %f)\n", float4[0], float4[1]);

    /*
    *CullFaceMode
    *#define GL_FRONT                                      0x0404
    *#define GL_BACK                                        0x0405
    *#define GL_FRONT_AND_BACK                 0x0408
    */
    glGetIntegerv(GL_CULL_FACE_MODE, integer4);
    printf("Cull face mode 0x%x\n", integer4[0]);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, integer4);
    printf("Max texture size: %d\n", integer4[0]);
    glGetIntegerv(GL_RED_BITS, integer4);
    printf("Red_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_GREEN_BITS, integer4);
    printf("Green_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_BLUE_BITS, integer4);
    printf("Blue_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_ALPHA_BITS, integer4);
    printf("Alpha_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_DEPTH_BITS, integer4);
    printf("Depth_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_DEPTH_BUFFER_BIT, integer4);
    printf("Depthbuffer_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_STENCIL_BITS, integer4);
    printf("Stencil_Bits:%d \n", integer4[0]);

    EGLint numOfConfigs = -1;
    eglGetConfigs(mEGLDisplay, NULL, 0, &numOfConfigs);
    printf("-----Number of EGL configs :%d-----\n", numOfConfigs);
    EGLConfig* configData = new EGLConfig[numOfConfigs];
    eglGetConfigs(mEGLDisplay, configData, numOfConfigs, &numOfConfigs);
    for(int i = 0; i < numOfConfigs; i++)
        {
            //printEGLConfigInformation(configData[i]);
        }
}

void RenderMachine::printEGLConfigInformation(EGLConfig config)
{
#define X(VAL) {VAL, #VAL}
    struct
    {
        EGLint attribute;
        const char* name;
    } names[] =
    {
        X(EGL_BUFFER_SIZE),
        X(EGL_RED_SIZE),
        X(EGL_GREEN_SIZE),
        X(EGL_BLUE_SIZE),
        X(EGL_ALPHA_SIZE),
        X(EGL_DEPTH_SIZE),
        X(EGL_STENCIL_SIZE),
        X(EGL_SAMPLES),
        X(EGL_SAMPLE_BUFFERS),
        X(EGL_SURFACE_TYPE),
        X(EGL_COLOR_BUFFER_TYPE),
        X(EGL_RENDERABLE_TYPE),
        X(EGL_CONFIG_CAVEAT),
        X(EGL_CONFIG_ID),
        X(EGL_LEVEL),
        X(EGL_MAX_PBUFFER_HEIGHT),
        X(EGL_MAX_PBUFFER_PIXELS),
        X(EGL_MAX_PBUFFER_WIDTH),
        X(EGL_NATIVE_RENDERABLE),
        X(EGL_NATIVE_VISUAL_ID),
        X(EGL_NATIVE_VISUAL_TYPE),
        X(EGL_TRANSPARENT_TYPE),
        X(EGL_TRANSPARENT_RED_VALUE),
        X(EGL_TRANSPARENT_GREEN_VALUE),
        X(EGL_TRANSPARENT_BLUE_VALUE),
        X(EGL_BIND_TO_TEXTURE_RGB),
        X(EGL_BIND_TO_TEXTURE_RGBA),
        X(EGL_MIN_SWAP_INTERVAL),
        X(EGL_MAX_SWAP_INTERVAL),
        X(EGL_LUMINANCE_SIZE),
        X(EGL_ALPHA_MASK_SIZE),
        X(EGL_CONFORMANT)
    };
#undef X
    printf("-----Configuration we have choosed!-----\n");
    for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); j++)
        {
            EGLint value = -1;
            EGLint returnVal = eglGetConfigAttrib(mEGLDisplay, config, names[j].attribute, &value);
            EGLint error = eglGetError();
            if (returnVal && error == EGL_SUCCESS)
                {
                    printf(" %s: ", names[j].name);
                    printf("%d (0x%x)\n", value, value);
                }
        }
    printf("\n");
}

void RenderMachine::printRenderingConfiguration()
{
    printf("-Render Configuration-\n \
   \t ===== Color pass ===== \n \
   \t hasColorConstantPass \t%d\n \
   \t hasColorDirectPass \t%d\n \
   \t hasColorDirectPassCombimeVBO %d\n \
   \t hasPreciMidium \t%d\n  \
   \n\
   \t ===== Affine transform ===== \n \
   \t hasRotation \t\t%d\n  \
   \t hasScale \t\t%d\n    \
   \t hasTranslation \t%d\n  \
   \t hasOrthoProjection \t%d\n \
   \t hasNDCVerticle \t%d\n \
   \n\
   \t ===== Color Image Processing =====\n \
   \t hasHuePP \t\t%d\n \
   \t hasBrightnessPP \t%d\n \
   \t hasContrastPP \t\t%d\n \
   \t hasSaturationPP \t%d\n \
   \t hasSharpnessPP \t%d\n \
   \t hasGrayscalePP \t%d\n \
   \n\
   \t ===== Light and texture ===== \n \
   \t hasLighting \t%d\n   \
   \t hasTexture2D \t%d\n  \
   \t hasMipMap \t%d\n \
   \n\
   \t ===== Buffer object ===== \n \
   \t hasBasicCubeFBO \t%d\n  \
   \t hasVAO \t\t%d\n \
   \n\
   \t ===== Polygon setup =====\n \
   \t hasSimTriangle \t%d\n \
   \t hasRectangle \t%d\n \
   \t hasCube\t%d\n \
   \t hasCubeWithVBO\t%d\n \
   \t hasCubeIndexModeWithoutVBO %d\n \
   \t hasBlenderObjectModel \t%d\n  \
   \n\
   \t ===== Fragment backend operation ===== \n \
   \t hasScissor\t\t%d\n  \
   \t hasBasicStencilOpe\t%d\n  \
   \t hasBasicDepthTest\t%d\n  \
   \t hasBasicMSAA\t\t%d\n  \
   \t hasBasicBlendingOpe\t%d\n  \
   \t hasCullFace\t\t%d\n \
   \n\
   \t ===== Image post-processing ===== \n \
   \t hasGaussianBlur\t%d\n\n",
           hasColorConstantPass,
           hasColorDirectPass,
           hasColorDirectPassCombimeVBO,
           hasPreciMidium,
           hasRotation,
           hasScale,
           hasTranslation,
           hasOrthoProjection,
           hasNDCVerticle,
           hasHuePP,
           hasBrightnessPP,
           hasContrastPP,
           hasSaturationPP,
           hasSharpnessPP,
           hasGrayscalePP,
           hasLighting,
           hasTexture2D,
           hasMipMap,
           hasBasicCubeFBO,
           hasVAO,
           hasSimTriangle,
           hasRectangle,
           hasCube,
           hasCubeWithVBO,
           hasCubeIndexModeWithoutVBO,
           hasBlenderObjectModel,
           hasScissor,
           hasBasicStencilOpe,
           hasBasicDepthTest,
           hasBasicMSAA,
           hasBasicBlendingOpe,
           hasCullFace,
           hasGaussianBlur
          );

}

GLuint RenderMachine::loadShader(GLenum shaderType, const char* pSource)
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
    else
        {
            error_print("Create shader error!\n");
        }
    return shader;
}

void RenderMachine::loadTexture(int* width, int* height, void** pixelData)
{
    /*
    *M.png was the PNG32 format under the /data/ directory.
    *Assume that we just support 8888 format for now and must be an NonOfPower size.
    *SkBitmap::kARGB_8888_Config:
    */

    //const char* fileName = "/data/M.png";
    //const char* fileName = "/data/DesertTreeCloud.png";   DarkModel-MiddBG.png
    //const char* fileName = "/data/MiddModel-MiddBG.png";
    const char* fileName = "/data/DarkModel-MiddBG.png";
    struct stat dest;
    if(stat(fileName, &dest) < 0)
        {
            error_print("Get the texture file size failed!");
            exit(-1);
        }

    /* Address and alloc the memory for pixel */
    unsigned int fileSize =  dest.st_size;
    void * imagePixels = malloc(fileSize);
    if(imagePixels == NULL)
        {
            error_print("imagePixels OOM!");
            exit(-1);
        }

    FILE * file = NULL;
    file = fopen(fileName, "r");
    if (file == NULL) printf("Open texture was faild!\n");
    unsigned int haveReaded = fread(imagePixels, 1, fileSize, file);
    if(fileSize == haveReaded) printf("Load %d bytes texture data was sucessful!\n", haveReaded);
    fclose(file);

    /* Encode */
    SkMemoryStream stream(imagePixels, fileSize);
    SkImageDecoder* codec = SkImageDecoder::Factory(&stream);
    codec->setDitherImage(false);
    if(codec)
        {
            codec->decode(&stream, &mBitmap, SkBitmap::kARGB_8888_Config, SkImageDecoder::kDecodePixels_Mode);
            delete codec;
            codec = NULL;
        }
    else
        {
            printf("Create codec was faild!\n");
        }

    *width = mBitmap.width();
    *height = mBitmap.height();
    *pixelData= mBitmap.getPixels();
    free(imagePixels);

    int p2Width = 1 << (31 - __builtin_clz(*width));
    int p2Height = 1 << (31 - __builtin_clz(*height));
    printf("   Texture dimension: width %d, height %d\n", *width, *height);

    if(p2Width != *width || p2Height != *height)
        {
            error_print("Non power of 2 size for Texture data!");
            /*
            *As the traditional OpenGL spec said,  Spec didn't support non-power of 2 texture size , but vairous implementation
            * can support it via his own extensions, So, we keep the maxium texture siza as screen size (e.g. 2013Nexus 7)
            */
            //exit(-1);
        }
}

GLuint RenderMachine::createProgram(const char* pVertexSource, const char* pFragmentSource)
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

void RenderMachine::polygonShaderSetup()
{
    /* Setup Vertext shader header */
    mVertexShader.append(mSG.gVS_Header_Attribute_vertexPosition);  //We always pass the vertex to the shader
    if(hasRotation) mVertexShader.append(mSG.gVS_Header_Uniform_rotationMatrix);
    if(hasScale) mVertexShader.append(mSG.gVS_Header_Uniform_scaleMatrix);
    if(hasTranslation) mVertexShader.append(mSG.gVS_Header_Uniform_translationMatrix);
    if(hasOrthoProjection) mVertexShader.append(mSG.gVS_Header_Uniform_OrthoProjcMatrix);
    if(hasColorDirectPass) mVertexShader.append(mSG.gVS_Header_Attribute_passColor);
    if(hasColorDirectPass) mVertexShader.append(mSG.gVS_Header_Varying_colorToFrag);
    if(hasTexture2D) mVertexShader.append(mSG.gVS_Header_Attribute_texCoord);
    if(hasTexture2D) mVertexShader.append(mSG.gVS_Header_Varying_texCoordToFrag);

    /* Setup Vertex shader main body */
    mVertexShader.append(mSG.gVS_Main_Start_Function);

    mVertexShader.append(mSG.gVS_Function_Direct_Pass_Position);
    if(hasRotation) mVertexShader.append(mSG.gVS_Function_Pass_RO_Multi_Position);
    if(hasScale) mVertexShader.append(mSG.gVS_Function_Pass_SC_Multi_Position);
    if(hasTranslation) mVertexShader.append(mSG.gVS_Function_Pass_TR_Multi_Position);
    if(hasOrthoProjection) mVertexShader.append(mSG.gVS_Function_OrthoProjection_Multi_Position);
    if(hasColorDirectPass) mVertexShader.append(mSG.gVS_Function_Pass_Color_To_Frag);
    if(hasTexture2D) mVertexShader.append(mSG.gVS_Function_Pass_texCoord_To_Frag);

    mVertexShader.append(mSG.gVS_Main_End_Function);
    printf("\nVertextShader=> \n");
    printf("%s\n", mVertexShader.string());

    /* Setup Fragment shader header */
    if(hasPreciMidium)mFramgmentShader.append(mSG.gFS_Header_Precision_Mediump_Float);
    if(hasColorDirectPass)mFramgmentShader.append(mSG.gFS_Header_Varying_colorToFrag);
    if(hasTexture2D) mFramgmentShader.append(mSG.gFS_Header_Varying_texCoordToFrag);
    if(hasTexture2D) mFramgmentShader.append(mSG.gFS_Header_Sampler2D);
    if(hasBrightnessPP) mFramgmentShader.append(mSG.gFS_Header_Brightness_Alpha);

    /* Setup Fragment shader main body */
    mFramgmentShader.append(mSG.gFS_Main_Start_Function);

    if(hasColorConstantPass)mFramgmentShader.append(mSG.gFS_Function_Pass_Constant_Color);
    if(hasColorDirectPass)mFramgmentShader.append(mSG.gFS_Function_Direct_Pass_Color);
    if(hasTexture2D) mFramgmentShader.append(mSG.gFS_Function_Direct_Sampler_texCoord);
    if(hasBrightnessPP) mFramgmentShader.append(mSG.gFS_Function_Brightness);
    if(hasGrayscalePP) mFramgmentShader.append(mSG.gFS_Function_Grayscale);

    mFramgmentShader.append(mSG.gFS_Main_End_Function);
    printf("FragmentShader=> \n");
    printf("%s\n", mFramgmentShader.string());

    if(hasCube || hasCubeWithVBO)
        {
            if(hasNDCVerticle)//Note: generateCube() will generate NDC based verticle
                {
                    mCubeNumOfIndex = VertexGenerator::generateCube(hasCubeIndexModeWithoutVBO ? true : false, 0.3,
                                      &mCubeVertices, NULL, &mCubeTexCoords, &mCubeColor, &mCubeIndices);
                }
            else
                {
                    error_print("Wrong configucation, Not allow!");
                    exit(-1);
                }
        }

    if(hasRectangle)//Note; generateRectangle will generate ScreenCoord based verticle
        VertexGenerator::generateRectangle(0.0f, surfaceHeight, 0.0f, 0.0f,
                                           surfaceWidth, 0.0f, surfaceWidth, surfaceHeight,
                                           &mRectangleVertices, &mRectangleTexCoords);
}

bool RenderMachine::polygonBuildnLink(const char vertexShader[], const char fragmentShader[])
{
    mOGLProgram= createProgram(vertexShader, fragmentShader);
    if (!mOGLProgram)
        {
            return false;
        }

    /* Retrieve shader paramer information */
    mAttrVSPosition = glGetAttribLocation(mOGLProgram, "a_vertexPosition");

    if(hasRotation)
        mUniVSrotateMat = glGetUniformLocation(mOGLProgram, "u_rotationMatrix");

    if(hasScale)
        mUniVSscaleMat = glGetUniformLocation(mOGLProgram, "u_scaleMatrix");

    if(hasTranslation)
        mUniVStranslateMat = glGetUniformLocation(mOGLProgram, "u_translationMatrix");

    if(hasOrthoProjection)
        mUniVSorthoprojecMat = glGetUniformLocation(mOGLProgram, "u_orthoProjecMatrix");

    if(hasColorDirectPass)
        mAttrVSColorPass = glGetAttribLocation(mOGLProgram, "a_passColor");

    /* Create various buffer object */
    if(hasCubeWithVBO)
        {
            glGenBuffers(3, mVBOForVI); //Note: This is just used for Index mode rendering
            //[0] for Vertex
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[0]);
            glBufferData(GL_ARRAY_BUFFER, VertexGenerator::vertexCubeSizeByte(true),
                         mCubeVertices, GL_STATIC_DRAW);

            //[1] for color
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[1]);
            glBufferData(GL_ARRAY_BUFFER, VertexGenerator::colorCubeSizeByte(true),
                         mCubeColor, GL_STATIC_DRAW);

            //[2] for index
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOForVI[2]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 500/*VertexGenerator::indexCubeSizeByte()*/,
                         mCubeIndices, GL_STATIC_DRAW);
        }

    if(hasTexture2D)
        {
            int width = 0, height =0;
            void* pixelData;

            glGenTextures(1, mTexture);
            glBindTexture(GL_TEXTURE_2D, mTexture[0]);

            loadTexture( &width, &height, &pixelData);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            if(hasMipMap)
                {
                    glGenerateMipmap(GL_TEXTURE_2D);
                }

            mAttrVSTexCoordPass = glGetAttribLocation(mOGLProgram, "a_texCoord");  /*Texture coordination for VS */
            mUniFSSampler = glGetUniformLocation(mOGLProgram, "u_samplerTexture"); /*Sampler unit for FS */

            glActiveTexture(GL_TEXTURE0);                                   /* Enable Texutre sampler unit */
            glBindTexture(GL_TEXTURE_2D, mTexture[0]);
            glUniform1i(mUniFSSampler, 0);
        }

    if(hasBrightnessPP)
        {
            mUniFSBrightnessAlpha= glGetUniformLocation(mOGLProgram, "u_brightnessAlpha");
        }


    /*
    *Cui. YY 20131008:W=window width, H=window height by default
    *As the NDC->SC formular, if w, h doesn't match as the same value in here,
    *It will be generate the stretch gemotry
    *
    *Ox = (x + w)/2
    *Oy = (y + h)/2
    *
    *Xw = (w/2)Xd + Ox
    *Yw = (h/2)Yd + Oy
    *Zw = ((f-n)/2)Zd + (n + f)/2
    *
    *N=0.0, F=1.0 by default
    *If set this as (0, 0), as the formulal there is no Z value will be considered while rasterization.
    *In order to keep geometry on the centre, keep the view post dimension as the same.
    *Note that the origin in view post was that left-botton based on each specific EGL surface.
    */

    /* ViewPort Setting */
    if(hasCube)//FixMe; TODO Will be remvoed in the future while NDC verticle was deprecated
        {
            glViewport(0, (surfaceHeight-surfaceWidth)/2, surfaceWidth, surfaceWidth);//FixMe; Is it really necessary ?
            glDepthRangef(0, 1);
        }

    /* Buffer Clear */
    glClearColor(gColorMatrix[mIndex][0], gColorMatrix[mIndex][1],
                 gColorMatrix[mIndex][2], gColorMatrix[mIndex][3]);

    glClearDepthf(0);
    glClearStencil(0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Scissor Test */
    if(hasScissor)
        {
            glScissor(0, (surfaceHeight-surfaceWidth)/2, surfaceWidth, surfaceWidth/2);
            glEnable(GL_SCISSOR_TEST);
        }

    /* Cull Face Test Culling should always be enabled to improve the perf of OpenGL, Culling back and CCW was defualt config */
    if(hasCullFace)
        {
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_CULL_FACE);
        }
    /* Stencil Test */
    if(hasBasicStencilOpe)
        {
            glStencilMask(0xFF);
            glEnable(GL_STENCIL_TEST);
        }
    /* Depth Test */
    if(hasBasicDepthTest)
        {
        }
    /* Blending Test */
    if(hasBasicBlendingOpe)
        {
        }
    /* Dithering Test */
    if(hasBasicMSAA)
        {
        }
    /* FBO, FBO what ?  color? texture? depth? stencil? */
    if(hasBasicCubeFBO)
        {
            int rboMaxSize;
            GLuint FBO;
            GLuint RBO;
            glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &rboMaxSize);
            printf("-----Maxium renderbuffer size :%d\n", rboMaxSize);
            glGenFramebuffers(1, &FBO);
            glGenRenderbuffers(1, &RBO);

            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 512, 512);

            glBindFramebuffer(GL_FRAMEBUFFER, FBO);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
            //glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
                {
                    /*
                    *
                    *Do something with this framebuffer
                    *
                    */
                }
        }

    glUseProgram(mOGLProgram);
    return true;
}

void RenderMachine::polygonDraw()
{
    glClear(GL_COLOR_BUFFER_BIT);

    /* ==== polygon vertex data update!==== */
    if(hasSimTriangle)
        {
            glVertexAttribPointer(mAttrVSPosition, 2, GL_FLOAT, GL_FALSE, 0, gSimpleTriangleVertices);
            glEnableVertexAttribArray(mAttrVSPosition);
        }

    if(hasRectangle)
        {
            glVertexAttribPointer(mAttrVSPosition, 2, GL_FLOAT, GL_FALSE, 0, mRectangleVertices);
            glEnableVertexAttribArray(mAttrVSPosition);
        }

    if(hasCubeWithVBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[0]);
            glVertexAttribPointer(mAttrVSPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(mAttrVSPosition);
        }

    if(hasCube)
        {
            glVertexAttribPointer(mAttrVSPosition, 3, GL_FLOAT, GL_FALSE, 0, mCubeVertices);
            glEnableVertexAttribArray(mAttrVSPosition);
        }

    /*
    *FixMe; TODO: Get the rid of updating uniform from this draw function
    *
    */

    /* ==== do transformantion ==== */
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
            MatrixTransform::matrixTranslate(&mTranslateMatrix, 0.0f, 0.0f, 0.0f);
            MatrixTransform::matrixDump(&mTranslateMatrix, "mTranslateMatrix");
            glUniformMatrix4fv(mUniVStranslateMat, 1, GL_FALSE, (GLfloat * )mTranslateMatrix.m);
        }

    if(hasOrthoProjection)
        {
            /* mUniVSorthoprojecMat */
            MatrixTransform::matrixIndentity(&mOrthoProjecMatrix);
            MatrixTransform::matrixOrthoProjection(&mOrthoProjecMatrix, 0, surfaceWidth, 0, surfaceHeight, 0, 1);
            MatrixTransform::matrixDump(&mOrthoProjecMatrix, "mOrthoProjecMatrix");
            glUniformMatrix4fv(mUniVSorthoprojecMat, 1, GL_FALSE, (GLfloat*)mOrthoProjecMatrix.m);
        }

    if(hasBasicStencilOpe)  /* Stencil Test */
        {
            glStencilFunc(GL_EQUAL, 0, 0x7);
            glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
            glDrawElements(GL_TRIANGLES, mCubeNumOfIndex, GL_UNSIGNED_INT, mCubeIndices);
        }

    /* ==== Buffer object update! ==== */
    if(hasColorDirectPassCombimeVBO)
        {
            if(hasColorDirectPass)
                {
                    /*
                    * hasColorDirectPass can use as standalone, but if want to use VBO version, should alwasy mark has-
                    * ColorDirectPassCombineVBO as true. Not allow only hasColorDirectPassCombineVBO condition as true.
                    */
                    glBindBuffer(GL_ARRAY_BUFFER, mVBOForVI[1]);
                    glVertexAttribPointer(mAttrVSColorPass, 4, GL_FLOAT, GL_FALSE, 0, 0);
                    glEnableVertexAttribArray(mAttrVSColorPass);
                }
            else
                {
                    error_print("Wrong configucation, Not allow!");
                    exit(-1);
                }
        }
    else if(hasColorDirectPass)
        {
            glVertexAttribPointer(mAttrVSColorPass, 4, GL_FLOAT, GL_FALSE, 0, mCubeColor);
            glEnableVertexAttribArray(mAttrVSColorPass);
        }

    /* ==== texture mapping update! ==== */
    if(hasTexture2D)
        {
            if(hasCube)
                {
                    glVertexAttribPointer(mAttrVSTexCoordPass, 2, GL_FLOAT, GL_FALSE, 0, mCubeTexCoords); /* Enable Texture coordination */
                }
            if(hasRectangle)
                {
                    glVertexAttribPointer(mAttrVSTexCoordPass, 2, GL_FLOAT, GL_FALSE, 0, mRectangleTexCoords);
                }
            glEnableVertexAttribArray(mAttrVSTexCoordPass);
        }

    /* ==== Color image post-processing ==== */
    /*
    *Linear interpolation and Extrapolation
    * Image_out = (1-a) * Image_target + a * Image_source
    */
    if(hasBrightnessPP)
        {
            if(mBrightnessMagnitude >= 2.0f)
                {
                    mBrightnessMagnitude = 0.0f;
                }
            else
                {
                    mBrightnessMagnitude += 0.002f;
                }
            //printf("mBrightnessMagnitude %f \n", mBrightnessMagnitude);
            glUniform1f(mUniFSBrightnessAlpha, mBrightnessMagnitude);
        }

    /* ==== draw something ==== */
    if(hasSimTriangle)
        {
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

    if(hasCubeWithVBO)
        {
            glDrawElements(GL_TRIANGLES, mCubeNumOfIndex, GL_UNSIGNED_INT, 0);
        }

    if(hasCube)
        {
            /* Stencil Test */
            if(hasBasicStencilOpe)
                {
                    glStencilFunc(GL_EQUAL, 1, 0x7);
                    glStencilOp(GL_KEEP, GL_DECR, GL_DECR);
                }
            /*
            *glDrawElements will run faster than glDrawArray for many reasons.
            */
            if(hasCubeIndexModeWithoutVBO) //Must exclusive used with VBO mode
                glDrawElements(GL_TRIANGLES, mCubeNumOfIndex, GL_UNSIGNED_INT, mCubeIndices);
            else
                glDrawArrays(GL_TRIANGLES, 0, 36); // For Cube drawing without index
        }

    if(hasRectangle)
        {
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

    /* ==== Swap something ==== */
    eglSwapBuffers(mEGLDisplay, mEGLSurface);
}


/*
*   Note: This is Static member function
*/
void RenderMachine::frameControl(int fd, int events, void* data)
{
    RenderMachine* thisObject = (RenderMachine*)data;
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
void* RenderMachine::mainRender(void* thisthis)
{
    RenderMachine * thisObject = (RenderMachine*)thisthis;
    printf("TID:%d Create render\n", gettid());

    EGLBoolean returnValue;
    EGLConfig myConfig = {0};
    EGLint numConfigs = 0;
    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    /*
    *Cui.YY 20131013; Decide to go to the Shanghai :-)
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
    printf("---We match %d number config(s)-----\n", numConfigs);
    //thisObject->printEGLConfigInformation(myConfig);

    /* EGL Create window surface and Context */
    thisObject-> mEGLSurface= eglCreateWindowSurface(thisObject->mEGLDisplay, myConfig, thisObject->windowSurface, NULL);
    context = eglCreateContext(thisObject->mEGLDisplay, myConfig, EGL_NO_CONTEXT, context_attribs);

    /* EGL Make current as available */
    returnValue = eglMakeCurrent(thisObject->mEGLDisplay, thisObject->mEGLSurface, thisObject->mEGLSurface, context);

    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_WIDTH, &(thisObject->surfaceWidth));
    eglQuerySurface(thisObject->mEGLDisplay, thisObject->mEGLSurface, EGL_HEIGHT, &(thisObject->surfaceHeight));
    printf("TID:%d Window dimensions: %d x %d\n", gettid(), thisObject->surfaceWidth, thisObject->surfaceHeight);

    /*
    *   1: Generating polygon shader program
    *   2: Build shader and link it
    *   3: Pass control over to Looper to draw each frame
    */
    thisObject->polygonShaderSetup();

    if(!thisObject->polygonBuildnLink(thisObject->mVertexShader.string(), thisObject->mFramgmentShader.string()))
        {
            fprintf(stderr, "Could not set up graphics.\n");
            return (void *)0;
        }

    thisObject->printOpenGLDriverInformation();

    /* Create loop to handle event */
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

void RenderMachine::startRender(EGLNativeWindowType window,
                                sp<SurfaceComposerClient> composerClient,
                                sp<SurfaceControl> control, int identity)
{
    pthread_t thread_status;

    windowSurface = window;
    mSurfaceComposerClient = composerClient;
    mSurfaceControl = control;
    mId = identity;

    pthread_create(&thread_status, NULL, RenderMachine::mainRender, (void*)(this));
}

}
