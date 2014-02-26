#ifndef  RENDERMACHINA_H_
#define RENDERMACHINA_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <sys/stat.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <utils/RefBase.h>
#include <utils/Looper.h>
#include <utils/String8.h>

#include <ui/FramebufferNativeWindow.h>
#include <gui/DisplayEventReceiver.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <core/SkBitmap.h>
#include <core/SkStream.h>
#include <core/SkImageDecoder.h>

#include "MatrixTransform.h"
#include "VertexGenerator.h"
#include "Logging.h"
#include "INI_Parser/ParseType.h"

/* Effect header */
#include "Effects/EffectBase.h"
#include "Effects/GrayscaleEffect.h"
#include "Effects/PureCanvasEffect.h"


using android::sp;
using android::wp;
using android::RefBase;
using android::Looper;
using android::status_t;
using android::IBinder;
using android::ISurfaceComposer;
using android::DisplayEventReceiver;
using android::DisplayInfo;
using android::String8;
using android::SurfaceComposerClient;
using android::SurfaceControl;
using android::Surface;

namespace RenderEvaluator
{
class RenderMachine:public virtual RefBase
{
public:
    EGLNativeWindowType mWindowSurface;

    RenderMachine(LayerRenderType layerInfo);
    void startRender(EGLNativeWindowType surface);
    static void* mainRender(void* thisthis);

private:
    void polygonEffectSelect();
    bool polygonViewSetup();
    bool polygonOnceRoutine();
    void polygonDraw();

    static void frameControl(int fd, int events, void* data);

    DisplayEventReceiver mDisplayEventReceiver;
    sp<Looper> mLoop;
    EGLSurface mEGLSurface;
    EGLDisplay mEGLDisplay;
    EGLint surfaceWidth;
    EGLint surfaceHeight;
    size_t mCounter = 1;
    nsecs_t mOldTimeStamp;
    LayerRenderType mLayerInfo;

    EffectBase* mEffect;
};

}

#endif
