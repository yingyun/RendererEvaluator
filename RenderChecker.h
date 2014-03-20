/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDERCHECKER_H_
#define RENDERCHECKER_H_

#include <EGL/egl.h>

#if USE_OPENGL_ES_VER == 30
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif USE_OPENGL_ES_VER == 20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "Pattern/Singleton.h"
#include "Logging.h"

namespace RenderEvaluator
{
class RenderChecker : public Singleton<RenderChecker>
{
public:
    RenderChecker() {}
    void checkGLDriver(EGLDisplay display);
    void checkEGLConfig(EGLConfig config, EGLDisplay display);
    void checkGLErrors(const char* description);
};
}
#endif
