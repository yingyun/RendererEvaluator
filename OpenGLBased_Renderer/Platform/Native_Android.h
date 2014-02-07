/*
*
*Cui.YY
*Description:
* Platform specific - Android based
*
*This class connected with SurfaceFlinger via Binder service, so we can simulate
*all the operation just like  android gfx render framework was doing.
*In the theory, it can provide more sanity control of layers and the behavior of
*surfaceflinger.
*/
#ifndef  PLATFORM_NATIVE_ANDROID_H_
#define PLATFORM_NATIVE_ANDROID_H_

#include <stdint.h>
#include <sys/types.h>
#include <vector>
#include <string>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposerClient.h>
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

#include "INI_Parser/ParseType.h"
#include "Logging.h"

using android::sp;
using android::wp;
using android::ProcessState;
using android::status_t;
using android::IBinder;
using android::ISurfaceComposer;
using android::DisplayInfo;
using android::String8;
using android::IPCThreadState;
using android::SurfaceComposerClient;
using android::ISurfaceComposerClient;
using android::SurfaceControl;
using android::Surface;
using std::vector;

#define BASE_ZORDER 0x40000000

namespace RenderEvaluator
{
class Native_Android:public IBinder::DeathRecipient
{
public:
    Native_Android();
    bool startService();
    void setLayerRender(LayerRenderType& layerInfo);
    sp<Surface> createNativeSurface();
    void threadPool();
    bool stopService();
    //Derived frome DeathRecipient pure virtual class
    virtual void binderDied(const wp<IBinder>& who);

    sp<SurfaceComposerClient>  mSurfaceComposerClient;
    vector< sp<SurfaceControl> > mSurfaceControl;
    vector< sp<Surface> > mSurface;

private:
    LayerRenderType mLayerInfo;
};

}

#endif
