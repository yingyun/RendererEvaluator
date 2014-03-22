/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDEREVALUATOR_EDGEDETECTION_H_
#define RENDEREVALUATOR_EDGEDETECTION_H_

#include <utils/String8.h>

#include "EffectBase.h"
#include "../Logging.h"

using android::String8;

namespace RenderEvaluator
{

typedef VertexGenerator::Mesh2D MESH;
typedef VertexGenerator::VEC2_F vec2f;

class EdgeDetection : public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    Matrix44 mProjectionMatrix;
    LayerRenderType mLayerInfo;
    SkBitmap mBitmap;
    MESH mRectMesh;

    GLfloat * vertexData;
    GLfloat * texCoordsData;
    GLuint texCoordsHandler;
    GLuint positionHandler;
    GLuint projectionHandler;
    GLuint samplerHandler;
    GLuint xPointSizeHandler;
    GLuint yPointSizeHandler;
    GLuint mProgram;
    GLuint texture[1];
    GLuint mVertexArrayObject;
    GLuint mVertexPositionBuffer;
    GLuint mTextureCoordsBuffer;

public:
    EdgeDetection(LayerRenderType layerInfo);
    virtual ~EdgeDetection();
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateParamsEvery();
};

}
#endif
