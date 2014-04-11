/*
*BlenderModel,, Vertex render with object format which was produced from Blender tool.
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#include "BlenderModel.h"

namespace RenderEvaluator
{

BlenderModel::BlenderModel(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
    mDegreeRotate = 0.0f;
}

BlenderModel::~BlenderModel()
{
    glDeleteBuffers(1, &mVertexPositionBuffer);
    glDeleteBuffers(1, &mNormalPositionBuffer);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

void BlenderModel::gen_updateVertexVBO()
{
    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);

    size_t vertexSizeByte = mVerticesNum * VERTEX_C * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexSizeByte, mVerticesData, GL_STATIC_DRAW);
    glVertexAttribPointer(mPositionHandler, VERTEX_C, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(mPositionHandler);
    GL_ERROR_CHECK("BlenderModel: Update Vertex VBO");
}

void BlenderModel::gen_updateNormalVBO()
{
    glGenBuffers(1, &mNormalPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalPositionBuffer);

    size_t normalSizeByte = mVerticesNum * NORMAL_C * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, normalSizeByte, mNormalData, GL_STATIC_DRAW);
    glVertexAttribPointer(mNormalHandler, NORMAL_C, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(mNormalHandler);
    GL_ERROR_CHECK("BlenderModel: Update Normal VBO");
}

void BlenderModel::retriveShaderVariableLocation()
{
    mPositionHandler = glGetAttribLocation(mProgram, "a_position");
    mNormalHandler = glGetAttribLocation(mProgram, "a_normal");
    mModelMatrixHandler = glGetUniformLocation(mProgram, "u_modelMatrix");
    mViewMatrixHandler = glGetUniformLocation(mProgram, "u_viewMatrix");
    mProjectionHandler = glGetUniformLocation(mProgram, "u_projection");
    mNormalMatrixHandler = glGetUniformLocation(mProgram, "u_normalMatrix");

    mLightHandler.directionLocation = glGetUniformLocation(mProgram, "u_light.direction");
    mLightHandler.ambientColorLocation = glGetUniformLocation(mProgram, "u_light.ambientColor");
    mLightHandler.diffuseColorLocation = glGetUniformLocation(mProgram, "u_light.diffuseColor");
    mLightHandler.specularColorLocation = glGetUniformLocation(mProgram, "u_light.specularColor");

    mMaterialHandler.ambientColorLocation = glGetUniformLocation(mProgram, "u_material.ambientColor");
    mMaterialHandler.diffuseColorLocation = glGetUniformLocation(mProgram, "u_material.diffuseColor");
    mMaterialHandler.specularColorLocation = glGetUniformLocation(mProgram, "u_material.specularColor");
    mMaterialHandler.specularExponentLocation = glGetUniformLocation(mProgram, "u_material.specularExponent");
}

void BlenderModel::setupLight_MaterialColor()
{
    glUniform3fv(mLightHandler.directionLocation, 1, light.direction);
    glUniform4fv(mLightHandler.ambientColorLocation, 1, light.ambientColor);
    glUniform4fv(mLightHandler.diffuseColorLocation, 1, light.diffuseColor);
    glUniform4fv(mLightHandler.specularColorLocation, 1, light.specularColor);

    glUniform4fv(mMaterialHandler.ambientColorLocation, 1, material.ambientColor);
    glUniform4fv(mMaterialHandler.diffuseColorLocation, 1, material.diffuseColor);
    glUniform4fv(mMaterialHandler.specularColorLocation, 1, material.specularColor);
    glUniform1f(mMaterialHandler.specularExponentLocation, material.specularExponent);
}

void BlenderModel::gen_uploadModelMatrix(float degree)
{
    MatrixTransform::getInstance().doMAT_Identify(&mModelMatrix);
    MatrixTransform::getInstance().doMAT_Rotate(&mModelMatrix, degree, 0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(mModelMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mModelMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update model matrix");
}

void BlenderModel::gen_uploadViewMatrix()
{
    MatrixTransform::getInstance().doMAT_Identify(&mViewMatrix);
    MatrixTransform::getInstance().doMAT_LookAt(&mViewMatrix, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(mViewMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mViewMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update view matrix");
}

void BlenderModel::gen_uploadNormallMatrix()
{
    Matrix44 mvMatrix;
    MatrixTransform::getInstance().doMAT_Multiply(&mvMatrix, &mModelMatrix, &mViewMatrix);
    MatrixTransform::getInstance().doMat_ExtractMat3FromMat4(&mNormalMatrix, &mvMatrix);
    glUniformMatrix3fv(mNormalMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mNormalMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update normal matrix");
}

void BlenderModel::gen_uploadProjectionMatrix()
{
    MatrixTransform::getInstance().doMAT_Identify(&mProjectionMatrix);
    const float aspect = (float)mLayerInfo.LayerWidth / (float)mLayerInfo.LayerHeight;
    MatrixTransform::getInstance().doMAT_PerspectiveProjection(&mProjectionMatrix,
            40.0f, aspect, 1.0f, 100.0f);
    glUniformMatrix4fv(mProjectionHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mProjectionMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update projection matrix");
}

void BlenderModel::gen_updateMNVPMatrix()
{
    gen_uploadModelMatrix(mDegreeRotate);
    gen_uploadViewMatrix();
    gen_uploadNormallMatrix();
    gen_uploadProjectionMatrix();
}

/* ----------------------------------- Helper Utility -------------------------------*/
#define VAO_SAVE \
    do{glGenVertexArrays(1, &mVertexArrayObject); glBindVertexArray(mVertexArrayObject);} while(false);
#define VAO_RESTORE \
    do{glBindVertexArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0);} while(false)
#define VAO_BEGIN \
    do{glBindVertexArray(mVertexArrayObject);} while(false);
#define VAO_END \
    do{glBindVertexArray(0);} while(false)
#define OBJECT_INIT \
    do{VertexGenerator::getInstance().loadObjModel(mLayerInfo.LayerObjectModel, &mVerticesData, NULL, &mNormalData, &mVerticesNum);} while(false)
#define OBJECT_RECYCLE \
    do{VertexGenerator::getInstance().unloadObjModel(&mVerticesData, NULL, &mNormalData);} while(false)

bool BlenderModel::updateShaderOnce()
{
    const char * vertexShader = "\
    attribute vec4 a_position;\n\
    attribute vec3 a_normal;\n\
    uniform mat4 u_modelMatrix;\n\
    uniform mat4 u_viewMatrix;\n\
    uniform mat4 u_projection;\n\
    uniform mat3 u_normalMatrix;\n\
    varying vec4 v_eye;\n\
    varying vec3 v_normal;\n\
    void main(void)\n\
    {\n\
        v_eye = -(u_viewMatrix * u_modelMatrix * a_position);\n\
        gl_Position = u_projection * -v_eye;\n\
        v_normal = u_normalMatrix * a_normal;\n\
    }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
    precision mediump float;\n\
    struct LightProperties\n\
    {\n\
        vec3 direction;\n\
        vec4 ambientColor;\n\
        vec4 diffuseColor;\n\
        vec4 specularColor;\n\
    };\n\
    struct MaterialProperties\n\
    {\n\
        vec4 ambientColor;\n\
        vec4 diffuseColor;\n\
        vec4 specularColor;\n\
        float specularExponent;\n\
    };\n\
    uniform LightProperties u_light;\n\
    uniform MaterialProperties u_material;\n\
    varying vec4 v_eye;\n\
    varying vec3 v_normal;\n\
    void main(void)\n\
    {\n\
	vec4 color = u_light.ambientColor * u_material.ambientColor;\n\
	vec3 normal = normalize(v_normal);\n\
	float nDotL = max(dot(u_light.direction, normal), 0.0);\n\
	if (nDotL > 0.0)\n\
	{\n\
		vec3 eye = normalize(v_eye.xyz);\n\
		vec3 reflection = reflect(-u_light.direction, normal);\n\
		float eDotR = max(dot(eye, reflection), 0.0);\n\
		color += u_light.diffuseColor * u_material.diffuseColor * nDotL;\n\
		float specularIntensity = 0.0;\n\
		if (eDotR > 0.0)\n\
		{\n\
			specularIntensity = pow(eDotR, u_material.specularExponent);\n\
		}\n\
		color += u_light.specularColor * u_material.specularColor * specularIntensity;\n\
	}\n\
	gl_FragColor = color;\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(
                   mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);

    return true;
}

bool BlenderModel::updateAttributeOnce()
{
    retriveShaderVariableLocation();
    setupLight_MaterialColor();
    gen_updateMNVPMatrix();

    return true;
}

bool BlenderModel::updateBufferOnce()
{
    OBJECT_INIT;

    VAO_SAVE {
        gen_updateVertexVBO();
        gen_updateNormalVBO();
     } VAO_RESTORE;

     OBJECT_RECYCLE;

    return true;
}

bool BlenderModel::updateParamsEvery()
{
    mDegreeRotate = static_cast<float>(static_cast<int>(mDegreeRotate + 10) % 360);
    gen_uploadModelMatrix(mDegreeRotate);
    gen_uploadNormallMatrix();

    return true;
}

bool BlenderModel::drawPolygonEvery()
{
    VAO_BEGIN {
        glDrawArrays(MESH::TRIANGLES, 0, mVerticesNum);
    } VAO_END;

    return true;
}

}

