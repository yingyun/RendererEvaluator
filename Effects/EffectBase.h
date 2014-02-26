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
