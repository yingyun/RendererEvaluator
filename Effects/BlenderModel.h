/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef RENDEREVALUATOR_BLENDERMODEL_H_
#define RENDEREVALUATOR_BLENDERMODEL_H_

#include <utils/String8.h>

#include "EffectBase.h"
#include "../Logging.h"

using android::String8;

namespace RenderEvaluator
{

typedef VertexGenerator::Mesh2D MESH;
typedef VertexGenerator::VEC2_F vec2f;
typedef VertexGenerator::OBJModel ObjectModel;

class BlenderModel: public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    Matrix44 mModelMatrix;
    Matrix44 mViewMatrix;
    Matrix44 mProjectionMatrix;
    LayerRenderType mLayerInfo;
    ObjectModel mOBJModel;

    GLuint mProgram;
    GLuint positionHandler;
    GLuint projectionHandler;
    GLuint modelMatrixHandler;
    GLuint viewMatrixHandler;
    GLuint mVertexArrayObject;
    GLuint mVertexPositionBuffer;

public:
    BlenderModel(LayerRenderType layerInfo);
    virtual ~BlenderModel();
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateFrameEvery();
};

}
#endif
