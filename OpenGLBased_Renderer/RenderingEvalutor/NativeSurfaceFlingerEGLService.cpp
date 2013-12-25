/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of Nativce EGL surface service.
*Detailed see the class header.
*
*/

#include <NativeSurfaceFlingerEGLService.h>

#define NUMOFSURFACE 6
#define SIZE_PAD 160
#define POSITION_PAD 80
#define DEFAULT_ZORDER 0x40000000
#define TEST_INDEX 3
#define NUMOFSURFACE 6

namespace android
{

NativeSurfaceFlingerEGLService::NativeSurfaceFlingerEGLService()
{
}

bool NativeSurfaceFlingerEGLService::startService(unsigned int numOfSurface)
{
    printf("Strat Native SurfaceFlinger EGL Service!\n");
    if(!( (1<=numOfSurface) && (numOfSurface<=NUMOFSURFACE)))
        {
            printf("Can't support out of range of 1 <= num of surfaces <= 6!\n");
            return false;
        }
    //Create Binder connection
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    //Connect with SurfaceFlinger
    sp<SurfaceComposerClient> composerClient = new SurfaceComposerClient();
    status_t error = composerClient ->linkToComposerDeath(this);
    if(error)
        {
            printf("Link to composer death failed %s\n", strerror(-error));
        }

    sp<IBinder> dtoken(SurfaceComposerClient::getBuiltInDisplay(
                           ISurfaceComposer::eDisplayIdMain));
    DisplayInfo dinfo;
    status_t status = SurfaceComposerClient::getDisplayInfo(dtoken, &dinfo);
    if(status)
        {
            printf("Get display info failed, this is wrong routine");
            return false;
        }

    /*
    *create the native surface
    *If want to create set center as base position, should make sure what
    *position_pad size half as size_pad.
    */
    unsigned int ww[NUMOFSURFACE];
    unsigned int hh[NUMOFSURFACE];

    for(unsigned int a =0; a < numOfSurface; a++)
        {
            ww[a] = dinfo.w - SIZE_PAD*a;
            hh[a] = dinfo.h - SIZE_PAD*a;
            /*Read format was determined by EGL Context Create*/
            mSurfaceControl[a] = composerClient->createSurface(String8::format("Surface-%d", a),
                                 ww[a], hh[a], PIXEL_FORMAT_RGBA_8888, eFXSurfaceNormal);
            SurfaceComposerClient::openGlobalTransaction();
            mSurfaceControl[a]->setLayer(DEFAULT_ZORDER + SIZE_PAD*a);
            mSurfaceControl[a]->setPosition(POSITION_PAD * a, POSITION_PAD*a);
            SurfaceComposerClient::closeGlobalTransaction();
            mIDs[a] = a;
            mSurface[a] = mSurfaceControl[a]->getSurface();
            printf("NSFES: Surface-%d, w:%d, h:%d, ZOrder:0x%x \n", a, ww[a], hh[a], DEFAULT_ZORDER + SIZE_PAD*a);
        }
    mSurfaceComposerClient = composerClient;
    return true;
}

void NativeSurfaceFlingerEGLService::threadPool()
{
    printf("TID:%d joinThreadPool, Enjoy it!\n", gettid());
    IPCThreadState::self()->joinThreadPool();
}

bool NativeSurfaceFlingerEGLService::stopService()
{
    printf("Stop Native SurfaceFlinger EGL Service!");

    for(unsigned int a = 0; a < NUMOFSURFACE; a++)
        {
            mSurface[a].clear();//Clear state
            mSurfaceControl[a].clear();//Destory  surface
        }
    IPCThreadState::self()->stopProcess();
    return true;
}

void NativeSurfaceFlingerEGLService::binderDied(const wp<IBinder>& who)
{
    printf("SurfaceFlinger was died!, Kill myself!");
    kill(getpid(), SIGKILL);
}

}
