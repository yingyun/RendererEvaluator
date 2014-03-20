/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef  RENDEREVALUATOR_TEXTUREGENERATOR_H
#define  RENDEREVALUATOR_TEXTUREGENERATOR_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <core/SkBitmap.h>
#include <core/SkStream.h>
#include <core/SkImageDecoder.h>

#include <string>

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

#define TEXTURE_PATH "/data/RenderEvaluator/"

class TextureGenerator : public Singleton<TextureGenerator>
{
public:
    TextureGenerator() {}
    void loadTexture(std::string texture, int* width, int* height, void** pixelData, SkBitmap& bitmap);
    void samplingMode(int minFilter, int magFilter, int sFilter, int tFilter);
};

}

#endif
