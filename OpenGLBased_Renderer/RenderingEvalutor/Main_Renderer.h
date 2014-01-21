/*
*MEIZU Technology. ZHUHAI 2013
*
*Cui.YY
*Description:
*RenderMachine class was used for rendering real OpenGL frame
*with standard EGL context.
*This is very common class, and easy to extend to gl1, gl3.
*In order to use Android Native surface  should be combined with
*NativeSurfaceFlingerEGLService class.
*
*Suggestion: This is the best candiadate to study shader programming.
*
*/
#ifndef  GL2_BASIC_RENDER_H
#define GL2_BASIC_RENDER_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/stat.h>

//TODO: Support GLES1
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
//TODO: Support GLES3

#include <utils/RefBase.h>
#include <utils/Looper.h>
#include <utils/String8.h>

#include <ui/FramebufferNativeWindow.h>
#include <gui/DisplayEventReceiver.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include "MatrixTransform.h"
#include "VertexGenerator.h"

#include <core/SkBitmap.h>
#include <core/SkStream.h>
#include <core/SkImageDecoder.h>

namespace android
{
class RenderMachine:public virtual RefBase
{
public:
    EGLNativeWindowType windowSurface; /* ANativeWindow* */
    sp<SurfaceComposerClient> mSurfaceComposerClient;
    sp<SurfaceControl> mSurfaceControl;
    int mId;

    RenderMachine(unsigned int index, unsigned int step);
    void startRender(EGLNativeWindowType window,
                     sp<SurfaceComposerClient> composerClient,
                     sp<SurfaceControl> control, int identity);
    static void* mainRender(void* windowSurface);   //static for function callback

private:

    /* ---------- Data Declare Area Start, Note it's a global member ---------- */

    //Each frame render color
    static GLclampf gColorMatrix[6][4];
    /* polygon info*/
    static GLfloat gSimpleTriangleVertices[6];

    GLfloat * mRectangleVertices;
    GLfloat * mRectangleTexCoords;

    GLfloat * mCubeVertices;
    GLfloat * mCubeColor;
    GLfloat * mCubeTexCoords;
    GLuint * mCubeIndices;
    GLuint mCubeNumOfIndex;

    GLfloat * mBlenderVertices;

    //Shader for vertex
    static const char * gVS_Header_Attribute_vertexPosition;  //Header
    static const char * gVS_Header_Uniform_rotationMatrix;
    static const char * gVS_Header_Uniform_scaleMatrix;
    static const char * gVS_Header_Uniform_translationMatrix;
    static const char * gVS_Header_Uniform_OrthoProjcMatrix;
    static const char * gVS_Header_Attribute_passColor;
    static const char * gVS_Header_Attribute_texCoord;
    static const char * gVS_Header_Varying_colorToFrag;
    static const char * gVS_Header_Varying_texCoordToFrag;

    static const char * gVS_Main_Start_Function;  //Body
    static const char * gVS_Function_Direct_Pass_Position;
    static const char * gVS_Function_Pass_RO_Multi_Position;
    static const char * gVS_Function_Pass_SC_Multi_Position;
    static const char * gVS_Function_Pass_TR_Multi_Position;
    static const char * gVS_Function_OrthoProjection_Multi_Position;
    static const char * gVS_Function_Pass_Color_To_Frag;
    static const char * gVS_Function_Pass_texCoord_To_Frag;
    static const char * gVS_Function_Gaussian_Blur;
    static const char * gVS_Main_End_Function;

    //Shader for fragment
    static const char * gFS_Header_Precision_Mediump_Float;  //Header
    static const char * gFS_Header_Varying_colorToFrag;
    static const char * gFS_Header_Varying_texCoordToFrag;
    static const char * gFS_Header_Sampler2D;
    static const char * gFS_Header_Brightness_Alpha;

    static const char * gFS_Main_Start_Function;  //Body
    static const char * gFS_Function_Pass_Constant_Color;
    static const char * gFS_Function_Direct_Pass_Color;
    static const char * gFS_Function_Direct_Sampler_texCoord;
    static const char * gFS_Function_Gaussian_Blur;
    static const char * gFS_Function_Brightness;
    static const char * gFS_Function_Grayscale;
    static const char * gFS_Main_End_Function;



    /* --- Attribute, Uniform handler Start, Note it's a class object property --- */

    //Shader for vertex
    GLuint mAttrVSPosition;
    GLuint mUniVSrotateMat;
    GLuint mUniVSscaleMat;
    GLuint mUniVStranslateMat;
    GLuint mUniVSorthoprojecMat;
    GLuint mAttrVSColorPass;
    GLuint mAttrVSTexCoordPass;


    //Shader for fragment
    GLuint mUniFSSampler;
    GLuint mUniFSBrightnessAlpha;



    /* ------- Rendering options Start, Not it's a class object property------- */
    /* Note:If add an new item, should also keep with polygonShaderSetup function */
    bool hasColorConstantPass;
    bool hasColorDirectPass;
    bool hasPreciMidium;
    bool hasRotation;
    bool hasScale;
    bool hasTranslation;
    bool hasOrthoProjection;
    bool hasNDCVerticle; //Exclusive used with ortho projection, if we has ortho NDC will be invalid in OpenGL, so it's just macth with pure transform

    bool hasLighting;
    bool hasTexture2D;
    bool hasMipMap;
    bool hasBasicCubeFBO;

    bool hasSimTriangle;
    bool hasRectangle;
    bool hasCube;
    bool hasCubeIndexModeWithoutVBO; /*Note: Just can combine used with Color Direct Pass, not available with texture 2d*/
    bool hasCubeWithVBO;
    /*Must combine used with haCubeWithVBO and hasColorDirectPass
    *Reason: If we use VBO as data  source, so should use all of the data from
    *the VBO buffer, include Vertex, Normal, Color, TexCoord...
    *ColorDirectPassCombineVBO depends on some ColorDirectPass shader
    *code and configuration.
    */
    bool hasColorDirectPassCombimeVBO;
    bool hasBlenderObjectModel;
    bool hasVAO;//OpenGL3.0 only
    bool hasScissor;
    bool hasBasicStencilOpe;
    bool hasBasicDepthTest;
    bool hasBasicMSAA;
    bool hasBasicBlendingOpe;
    bool hasGaussianBlur;
    bool hasCullFace;

    /* Various post-processing shader effect */
    bool hasHuePP;

    /* Below four operation can complete by linear interpolation or extrapolation
    * Image_out = (1-a) * Image_target + a * Image_source
    */
    bool hasBrightnessPP;
    bool hasContrastPP;
    bool hasSaturationPP;
    bool hasSharpnessPP;
    bool hasGrayscalePP;

    /* bool hasGammaPP; */  //Is it possible in software side ?

    /* Various Testing/Evaluate function */
    bool hasGoogleTest;
    bool hasGPUEvaluation;

    /* ------ Various buffer object Start------ */
    GLuint mVBOForVI[3];
    GLuint mTexture[1];

    /* ------ Various transform matrix  Start------ */
    Matrix44 mRotateMatrix;
    Matrix44 mScaleMatrix;
    Matrix44 mTranslateMatrix;
    Matrix44 mOrthoProjecMatrix;

    //global shader source, class object property
    String8 mVertexShader;
    String8 mFramgmentShader;

    //Polygon render function
    void polygonShaderSetup();
    bool polygonBuildnLink(const char vertexShader[], const char fragmentShader[]);
    void polygonDraw();

    /*Cui.YY
     * static constexpr GLclampf gColorMatrix[6][4];  in class static member initialization,
     * just only available in C++11 standard, must combine used with constexpr keyword
     */
    unsigned int mIndex;
    unsigned int mStep;
    unsigned int mCounter;
    GLuint mOGLProgram;
    nsecs_t mOldTimeStamp;
    SkBitmap mBitmap;

    //Each transformation middle variable
    unsigned int mRotationAngle;
    GLfloat mScaleMagnitude;
    bool mScaleIncreasing;
    GLfloat mTranslationMagnitude;

    //Each post processing middle variable
    GLfloat mBrightnessMagnitude;

    DisplayEventReceiver mDisplayEventReceiver;
    sp<Looper> mLoop;
    EGLSurface mEGLSurface;
    EGLDisplay mEGLDisplay;
    EGLint surfaceWidth;
    EGLint surfaceHeight;

    //Used to setup basic rendering environment
    GLuint loadShader(GLenum shaderType, const char* pSource);
    void loadTexture(int* width, int* height, void** pixelData);

    GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
    static void frameControl(int fd, int events, void* data);  //static for function callback

    void printOpenGLDriverInformation();
    void printEGLConfigInformation(EGLConfig config);

};

}

#endif
