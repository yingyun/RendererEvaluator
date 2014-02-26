#include "Native_Android.h"

namespace RenderEvaluator
{
Native_Android::Native_Android()
{
}

bool Native_Android::startService()
{
    LOG_WARNING("Strat Native SurfaceFlinger EGL Service!\n");

    /* Establish binder connection */
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    /* Connect with SurfaceFlinger */
    mSurfaceComposerClient = new SurfaceComposerClient();
    status_t error = mSurfaceComposerClient ->linkToComposerDeath(this);
    if(error) LOG_ERROR("Link to composer death failed %s\n", strerror(-error));
    sp<IBinder> dtoken(SurfaceComposerClient::getBuiltInDisplay(ISurfaceComposer::eDisplayIdMain));
    DisplayInfo dinfo;
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    if(status)
        {
            LOG_ERROR("Get display info failed, this is wrong routine");
            return false;
        }
    return true;
}

void Native_Android::setLayerRender(LayerRenderType & layerInfo)
{
    mLayerInfo = layerInfo;
}

sp<Surface> Native_Android::createNativeSurface()
{
    mSurfaceControl.push_back(mSurfaceComposerClient->createSurface(String8::format("Surface-%d", mLayerInfo.LayerIndex),
                              mLayerInfo.LayerWidth, mLayerInfo.LayerHeight,
                              android::PIXEL_FORMAT_RGBA_8888, ISurfaceComposerClient::eFXSurfaceNormal));

    SurfaceComposerClient::openGlobalTransaction();
    mSurfaceControl.back()->setLayer(BASE_ZORDER+ mLayerInfo.LayerOrder);
    mSurfaceControl.back()->setPosition(mLayerInfo.LayerPositionX, mLayerInfo.LayerPositionY);
    SurfaceComposerClient::closeGlobalTransaction();

    mSurface.push_back(mSurfaceControl[mLayerInfo.LayerIndex]->getSurface());

    LOG_INFO("Surface %d; Position: %d, %d; Size: %d, %d;\n",
             mLayerInfo.LayerIndex, mLayerInfo.LayerPositionX, mLayerInfo.LayerPositionY,
             mLayerInfo.LayerWidth, mLayerInfo.LayerHeight);
    return mSurface.back();

}

void Native_Android::threadPool()
{
    LOG_INFO("TID:%d joinThreadPool, Enjoy it!\n", gettid());
    IPCThreadState::self()->joinThreadPool();
}

bool Native_Android::stopService()
{
    LOG_INFO("Stop Native SurfaceFlinger EGL Service!");
    IPCThreadState::self()->stopProcess();
    return true;
}

void Native_Android::binderDied(const wp<IBinder>& who)
{
    LOG_WARNING("SurfaceFlinger was died!, Kill myself!");
    kill(getpid(), SIGKILL);
}

}
