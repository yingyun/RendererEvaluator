/*
 *2013-2014 Cui.Yingyun
 * cuiyingyun@gmail.com
 * TODO: The description of this program purpose
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "NativeSurfaceFlingerEGLService.h"
#include "Main_Renderer.h"

#define NUMOFSURFACE 6

using namespace android;

void usage()
{
    /* FixMe; TODO: Change the note accordingly */
    printf("\n");
    printf("Cui.YY Native_RenderMachine Ver20130905\n");
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

    /*
     * TODO: Re-factor this argument parser, ugly! Out of date !
     * ---Use config file style to control the render---
     *OpenSource Bakcup:
     *http://www.hyperrealm.com/libconfig/               ;;library
     *http://www.gtkbook.com/tutorial.php?page=keyfile   ;;For GTK based desktop, Glib
     *https://code.google.com/p/inih/                    ;;Simple INI parser; Use it !
     *Use INIH R29 class to parse the config file in here.
     */
    if (argc == 3)
        {
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
            /* No protect here */
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
    surface->startService(num_of_surface);//Create EGL surface
    sp<RenderMachine> render[NUMOFSURFACE]= {0};

    for(unsigned int a =0; a < num_of_surface; a++)
        {
            render[a] = new RenderMachine(a, num_of_fps);
            render[a]->startRender(surface->mSurface[a].get(), surface->mSurfaceComposerClient,
                                   surface->mSurfaceControl[a], surface->mIDs[a]);
            sleep(1);
        }

    surface->threadPool();
    surface->stopService();

    return 0;
}
