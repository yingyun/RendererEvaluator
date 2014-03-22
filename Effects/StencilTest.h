/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDEREVALUATOR_STENCILTESTEFFECT_H_
#define RENDEREVALUATOR_STENCILTESTEFFECT_H_

#include <utils/String8.h>

#include "EffectBase.h"
#include "../Logging.h"

using android::String8;

namespace RenderEvaluator
{
#define NumTests  4

class StencilTest : public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    LayerRenderType mLayerInfo;

    static GLfloat vVertices[];
    static GLubyte indices[][6];
    static GLfloat  colors[NumTests][4];

    GLuint colorHandler;
    GLuint positionHandler;
    GLuint mProgram;

public:
    StencilTest(LayerRenderType layerInfo);
    virtual ~StencilTest();
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateParamsEvery();
};

}
#endif
