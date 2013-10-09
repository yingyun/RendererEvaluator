/*
*MEIZU Technology. ZHUHAI 2013
*
*Cui.YY
*Description:
*This class connected with SurfaceFlinger via Binder service, so we can simulate
*all the operation just like  android gfx render framework was doing.
*In the theory, it can provide more sanity control of layers and the behavior of
*surfaceflinger.
*
*
*
*/
#ifndef  NATIVESURFACEFLINGEREGLSERVICE_H
#define NATIVESURFACEFLINGEREGLSERVICE_H

#include <stdint.h>
#include <sys/types.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <utils/Errors.h>
#include <utils/Log.h>

#include <ui/PixelFormat.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <ui/DisplayInfo.h>
#include <ui/FramebufferNativeWindow.h>

#define NUMOFSURFACE 6

namespace android
{

class Surface;
class SurfaceComposerClient;
class SurfaceControl;


// flags for createSurface()
enum   // (keep in sync with Surface.java)
{
    eHidden             = 0x00000004,
    eDestroyBackbuffer  = 0x00000020,
    eSecure             = 0x00000080,
    eNonPremultiplied   = 0x00000100,
    eOpaque             = 0x00000400,
    eProtectedByApp     = 0x00000800,
    eProtectedByDRM     = 0x00001000,

    eFXSurfaceNormal    = 0x00000000,
    eFXSurfaceBlur      = 0x00010000, // deprecated, same as Dim
    eFXSurfaceDim       = 0x00020000,
    eFXSurfaceScreenshot= 0x00030000,
    eFXSurfaceRefer     = 0x00040000,
    eFXSurfaceMask      = 0x000F0000,
};

class NativeSurfaceFlingerEGLService:public IBinder::DeathRecipient
{
public:
    NativeSurfaceFlingerEGLService();
    bool startService(unsigned int numOfSurface);
    void threadPool();
    bool stopService();
    //Derived frome DeathRecipient pure virtual class
    virtual void binderDied(const wp<IBinder>& who);

    sp<SurfaceComposerClient>  mSurfaceComposerClient;
    sp<SurfaceControl> mSurfaceControl[NUMOFSURFACE];
    sp<Surface> mSurface[NUMOFSURFACE];
};

}//Namespace of android

#endif
