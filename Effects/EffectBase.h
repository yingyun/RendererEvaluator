/*
*Base class for filter to implement the function which they want to achive
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDEREVALUATOR_EFFECTS_EFFECTBASE_H_
#define RENDEREVALUATOR_EFFECTS_EFFECTBASE_H_

#include "../VertexGenerator.h"
#include "../MatrixTransform.h"
#include "../TextureGenerator.h"
#include "../ShaderProgramBuilder.h"
#include "../RenderChecker.h"
#include "../INI_Parser/ParseType.h"

namespace RenderEvaluator
{
class EffectBase
{
public:
    virtual ~EffectBase() {}
    virtual bool updateShaderOnce() = 0;
    virtual bool updateAttributeOnce() = 0;
    virtual bool updateBufferOnce() = 0;
    virtual bool drawPolygonEvery() = 0;
    virtual bool updateFrameEvery() = 0;
};
}
#endif
