/*
 *MEIZU Technology. ZHUHAI 2013
 *
 *The purpose of this program:
 *1:Measue surfaceflinger performance
 *2:Measure the impact of layer composer on power consumtion
 *3:Measure the performance of graphic buffer allocation
 *
 *20130819:Initial version of Native opengl program, basically it's based on SurfaceFlinger service.
 *In the google original design, it's use raw NativeWindow to control buffer deque and enque ope
 *ration via EGL driver. But it's doesn't have compatbility across different android version which
 *android was made huge change on graphic subsystem. So, i have to chang this as follow
 *SurfaceFlinger rule to get the buffer service from SurfaceFlinger.
 *
 *Note that it's just a snipe code of opengl 2.0. Just used for studying SF usage.
 *
 *20130824: Maybe it can be used for testing Native graphic performance.
 *If possible it also can integrate with Google Test Framework under external/gtest
 *
 *20130831: Can run with multiple OpenGL context thread
 *
 *20130901: Render each frame on each vsync event
 *
 *TODO
 *20130911: Add various effect on the canvas.
 * Goal: Control each frame with different remdering method via command cotnrol.
 *  =>Transformation, RTS
 *  =>Texture Mapping
 *  =>Lighting
 *  =>Various Buffer Control
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "NativeSurfaceFlingerEGLService.h"
#include "gl2_basic_render.h"

#define NUMOFSURFACE 6

using namespace android;


void usage()
{
    printf("\n");
    printf("Cui.YY Native_gl2_basic_render Ver20130905\n");
    printf("Usage: \"Native_gl2_basic 4 2 1 1 1 1\"\n");
    printf("4: means create 4 layes, the range from 1 to 6.\n");
    printf("2: means render each frame on each 2 vsync was triggered.\n ");
    printf("  the range from 1 to 60. Argument 2 and 3 must be combined use.\n");
    printf("1: means draw what kind of context into the canvas.\n");
    printf("        1=> simple Triangle draw\n");  //FixMe; change help
    printf("        2=> simple Triangle draw\n");
    printf("\n");
}

int main(int argc, char** argv)
{
    unsigned int num_of_surface = NUMOFSURFACE;
    unsigned int num_of_fps = 1;
    if (argc == 3)
        {
            //            usage();

            int b = atoi(argv[1]);
            if((1 <= b) && (b <=6))
                {
                    num_of_surface = b;
                }
            else
                {
                    printf("You have inputed wrong number.\n");
                    usage();
                    return 0;
                }
            int a = atoi(argv[2]);
            //No protect here
            num_of_fps = a;

            printf("=>Using %d layer(s) from the layer pool\n", num_of_surface);
            printf("=>Trigger render frame on each %d vsync signal\n", num_of_fps);
            printf("\n");
        }
    else if (argc == 1)
        {
            usage();
            return 0;
        }

    sp<NativeSurfaceFlingerEGLService> surface = new NativeSurfaceFlingerEGLService();
    surface->startService();//Create EGL surface

    sp<gl2_basic_render> render[NUMOFSURFACE]= {0};
    for(unsigned int a =0; a < num_of_surface; a++)
        {
            render[a] = new gl2_basic_render(a, num_of_fps);
            render[a]->startRender(surface->mSurface[a].get());
            sleep(1);
        }
    surface->threadPool();
    surface->stopService();
    return 0;
}
