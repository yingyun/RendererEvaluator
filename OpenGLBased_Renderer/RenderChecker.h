/*
* 20140129 Cui.Yingyun
*/

#ifndef RENDERCHECKER_H_
#define RENDERCHECKER_H_

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "Logging.h"

namespace RenderEvaluator
{
class RenderChecker
{
public:
    RenderChecker()
    {
        //Do something in the future
    }

    static void checkGLDriver(EGLDisplay display);
    static void checkEGLConfig(EGLConfig config, EGLDisplay display);
    static void checkGLErrors();
};
}
#endif
