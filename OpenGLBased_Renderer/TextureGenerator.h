/*
 * 20140129 Cui.Yingyun
 */

#ifndef  TEXTUREGENERATOR_H
#define  TEXTUREGENERATOR_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <core/SkBitmap.h>
#include <core/SkStream.h>
#include <core/SkImageDecoder.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "Logging.h"

namespace RenderEvaluator
{
class TextureGenerator
{
public:
    static void loadTexture(int* width, int* height, void** pixelData, SkBitmap& bitmap);
    static void samplingMode(int minFilter, int magFilter, int sFilter, int tFilter);
};

}

#endif
