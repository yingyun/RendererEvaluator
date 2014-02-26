#ifndef RENDEREVALUATOR_GRAYSCALEEFFECT_H_
#define RENDEREVALUATOR_GRAYSCALEEFFECT_H_

#include <utils/String8.h>
#include <core/SkBitmap.h>

#include "EffectBase.h"
#include "../Logging.h"

using android::String8;

namespace RenderEvaluator
{

class GrayscaleEffect : public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    Matrix44 mProjectionMatrix;
    LayerRenderType mLayerInfo;
    SkBitmap mBitmap;

    GLfloat * vertexData;
    GLfloat * texCoordsData;
    GLuint texCoordsHandler;
    GLuint positionHandler;
    GLuint projectionHandler;
    GLuint samplerHandler;
    GLuint colorOffsetHandler;
    GLuint mProgram;
    GLuint texture[1];

public:
    GrayscaleEffect(LayerRenderType layerInfo);
    virtual ~GrayscaleEffect() {}
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateFrameEvery();
};
}
#endif
