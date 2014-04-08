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

class BlenderModel: public EffectBase
{
private:
    typedef struct
    {
        GLfloat direction[3];
        GLfloat ambientColor[4];
        GLfloat diffuseColor[4];
        GLfloat specularColor[4];
    } LightProperties;

    typedef struct
    {
        GLfloat ambientColor[4];
        GLfloat diffuseColor[4];
        GLfloat specularColor[4];
        GLfloat specularExponent;
    } MaterialProperties;

    typedef struct
    {
        GLint directionLocation;
        GLint ambientColorLocation;
        GLint diffuseColorLocation;
        GLint specularColorLocation;
    } LightLocations;

    typedef struct
    {
        GLint ambientColorLocation;
        GLint diffuseColorLocation;
        GLint specularColorLocation;
        GLint specularExponentLocation;
    } MaterialLocations;

    String8 mFragShader;
    String8 mVertexShader;
    Matrix44 mModelMatrix;
    Matrix44 mViewMatrix;
    Matrix44 mProjectionMatrix;
    Matrix33 mNormalMatrix;
    LayerRenderType mLayerInfo;

    float* mVerticesData;
    float* mNormalData;
    unsigned int mVerticesNum;

    /*
    *TODO: Change it dynamically???
    */
    LightProperties light =
    {
        { 1.0f, 1.0f, 1.0f },
        { 0.3f, 0.3f, 0.3f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
    };

    MaterialProperties material =
    {
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        20.0f
    };

    GLuint mProgram;
    GLuint mPositionHandler;
    GLuint mNormalHandler;
    GLuint mProjectionHandler;
    GLuint mModelMatrixHandler;
    GLuint mNormalMatrixHandler;
    GLuint mViewMatrixHandler;
    GLuint mVertexArrayObject;
    GLuint mVertexPositionBuffer;
    GLuint mNormalPositionBuffer;
    LightLocations mLightHandler;
    MaterialLocations mMaterialHandler;

    void retriveShaderVariableLocation();
    void setupLight_MaterialColor();
    void gen_updateMNVPMatrix();
    void gen_updateVertexVBO();
    void gen_updateNormalVBO();
    void gen_updateModelMatrix();
    void gen_uploadViewMatrix();
    void gen_uploadNormallMatrix();
    void gen_uploadProjectionMatrix();

public:
    BlenderModel(LayerRenderType layerInfo);
    virtual ~BlenderModel();
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateParamsEvery();
};

}
#endif
