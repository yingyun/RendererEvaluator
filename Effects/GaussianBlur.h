/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDEREVALUATOR_GAUSSIANBLUR_H_
#define RENDEREVALUATOR_GAUSSIANBLUR_H_

#include <utils/String8.h>
#include <vector>
#include <math.h>
#include "EffectBase.h"
#include "../Logging.h"

using android::String8;
using std::vector;

namespace RenderEvaluator
{

typedef VertexGenerator::Mesh2D MESH;
typedef VertexGenerator::VEC2_F vec2f;

class GaussianBlur : public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    Matrix44 mProjectionMatrix;
    LayerRenderType mLayerInfo;
    SkBitmap mBitmap;
    MESH mRectMesh;
    vector<double> mWeights;

    GLfloat * vertexData;
    GLfloat * texCoordsData;
    GLuint texCoordsHandler;
    GLuint positionHandler;
    GLuint projectionHandler;
    GLuint samplerHandler;
    GLuint w0Handler;
    GLuint w1Handler;
    GLuint w2Handler;
    GLuint w3Handler;
    GLuint w4Handler;
    GLuint mProgram;
    GLuint stepHanlder;
    GLuint texture[1];
    GLuint mVertexArrayObject;
    GLuint mVertexPositionBuffer;
    GLuint mTextureCoordsBuffer;

public:
    GaussianBlur(LayerRenderType layerInfo);
    virtual ~GaussianBlur();
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateParamsEvery();
};

}
#endif
