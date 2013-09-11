/*
*MEIZU Technology. ZHUHAI 2013
*
*Cui.YY
*Description:
*gl2_basic_render class was used for rendering real OpenGL frame
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

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <utils/RefBase.h>
#include <utils/Looper.h>

#include <ui/FramebufferNativeWindow.h>
#include <gui/DisplayEventReceiver.h>

namespace android
{
class gl2_basic_render:public virtual RefBase
{
public:
    EGLNativeWindowType windowSurface;

    gl2_basic_render(unsigned int index, unsigned int step);
    void startRender(EGLNativeWindowType windowSurface);
    static void* mainRender(void* windowSurface);

private:
    //Each frame render color
    static GLclampf mColorMatrix[6][4];

    //simple Triangle Render
    static GLfloat mSimpleTriangleVertices[6];
    static char mSimpleTriangleVertexShader[];
    static char mSimpleTriangleFragmentShader[];
    bool polygonSetup(int w, int h, const char vertexShader[], const char fragmentShader[]);
    void polygonDraw();


    /*Cui.YY
    static constexpr GLclampf mColorMatrix[6][4];  in class static member initialization,
    just only available in C++11 standard, must combine used with constexpr keyword*/
    unsigned int mIndex;
    unsigned int mStep;
    unsigned int mCounter;
    GLuint gProgram;
    GLuint gvPositionHandle;
    nsecs_t oldTimeStamp;

    DisplayEventReceiver mDisplayEventReceiver;
    sp<Looper> mLoop;
    EGLSurface surface;
    EGLDisplay dpy;

    //Used to setup basic rendering environment
    GLuint loadShader(GLenum shaderType, const char* pSource);
    GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
    static void frameControl(int fd, int events, void* data);
    void frameSetup();
    void frameDraw();

};

}

#endif
