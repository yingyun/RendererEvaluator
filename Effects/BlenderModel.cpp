/*
*BlenderModel,, Vertex render with object format which was produced from Blender tool.
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#include "BlenderModel.h"

/*
TODO:
    1: Add light(Blinn-Phong)
    2: Add model or view change every frame!
    3: Refactor routine architecture
*/

namespace RenderEvaluator
{

BlenderModel::BlenderModel(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

BlenderModel::~BlenderModel()
{
    /*Release source*/
    glDeleteBuffers(1, &mVertexPositionBuffer);
    glDeleteBuffers(1, &mNormalPositionBuffer);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

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
        vec4 vertex;\n\
        vertex = u_modelMatrix * a_position;\n\
        vertex = u_viewMatrix * vertex;\n\
        v_eye = -vec4(vertex);\n\
        v_normal = u_normalMatrix * a_normal;\n\
        gl_Position =  u_projection * vertex;\n\
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
    /*Get the attribute from GLSL*/
    positionHandler = glGetAttribLocation(mProgram, "a_position");
    normalHandler = glGetAttribLocation(mProgram, "a_normal");
    modelMatrixHandler = glGetUniformLocation(mProgram, "u_modelMatrix");
    viewMatrixHandler = glGetUniformLocation(mProgram, "u_viewMatrix");
    projectionHandler = glGetUniformLocation(mProgram, "u_projection");
    normalMatrixHandler = glGetUniformLocation(mProgram, "u_normalMatrix");

    g_light.directionLocation = glGetUniformLocation(mProgram, "u_light.direction");
    g_light.ambientColorLocation = glGetUniformLocation(mProgram, "u_light.ambientColor");
    g_light.diffuseColorLocation = glGetUniformLocation(mProgram, "u_light.diffuseColor");
    g_light.specularColorLocation = glGetUniformLocation(mProgram, "u_light.specularColor");

    g_material.ambientColorLocation = glGetUniformLocation(mProgram, "u_material.ambientColor");
    g_material.diffuseColorLocation = glGetUniformLocation(mProgram, "u_material.diffuseColor");
    g_material.specularColorLocation = glGetUniformLocation(mProgram, "u_material.specularColor");
    g_material.specularExponentLocation = glGetUniformLocation(mProgram, "u_material.specularExponent");

    /*Update light and material color property*/
    glUniform3fv(g_light.directionLocation, 1, light.direction);
    glUniform4fv(g_light.ambientColorLocation, 1, light.ambientColor);
    glUniform4fv(g_light.diffuseColorLocation, 1, light.diffuseColor);
    glUniform4fv(g_light.specularColorLocation, 1, light.specularColor);

    glUniform4fv(g_material.ambientColorLocation, 1, material.ambientColor);
    glUniform4fv(g_material.diffuseColorLocation, 1, material.diffuseColor);
    glUniform4fv(g_material.specularColorLocation, 1, material.specularColor);
    glUniform1f(g_material.specularExponentLocation, material.specularExponent);

    /*
    LOG_INFO("Render=> a_position %d, u_modelMatrix %d, u_viewMatrix %d, u_projection %d\n",
             positionHandler, modelMatrixHandler, viewMatrixHandler, projectionHandler);
             */

    /*Update model matrix*/
    LOG_INFO("BlenderModel: Generate model matrix\n");
    MatrixTransform::getInstance().doMAT_Identify(&mModelMatrix);
    MatrixTransform::getInstance().doMAT_Rotate(&mModelMatrix, 10.0f, 0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(modelMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mModelMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update model matrix");

    /*Update view matrix*/
    LOG_INFO("BlenderModel: Generate view matrix\n");
    MatrixTransform::getInstance().doMAT_Identify(&mViewMatrix);
    MatrixTransform::getInstance().doMAT_LookAt(&mViewMatrix, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(viewMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mViewMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update view matrix");

    /*Update normal matrix*/
    LOG_INFO("BlenderModel: Generate normal matrix\n");
    Matrix44 mvMatrix;
    MatrixTransform::getInstance().doMAT_Multiply(&mvMatrix, &mModelMatrix, &mViewMatrix);
    MatrixTransform::getInstance().doMat_ExtractMat3FromMat4(&mvMatrix, &mNormalMatrix);
    glUniformMatrix3fv(normalMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mNormalMatrix.m));

    /*Update projection matrix*/
    LOG_INFO("BlenderModel: Generate projection matrix\n");
    MatrixTransform::getInstance().doMAT_Identify(&mProjectionMatrix);
    const float aspect = (float)mLayerInfo.LayerWidth / (float)mLayerInfo.LayerHeight;
    MatrixTransform::getInstance().doMAT_PerspectiveProjection(&mProjectionMatrix,
            40.0f, aspect, 1.0f, 100.0f);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mProjectionMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update projection matrix");

    return true;
}

bool BlenderModel::updateBufferOnce()
{

    VertexGenerator::getInstance().loadObjModel(mLayerInfo.LayerObjectModel,
            &mVerticesData, NULL, &mNormalData, &mVerticesNum);

    /*Update VAO*/
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    /*Update Vertex VBO*/
    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);

    size_t vertexSizeByte = mVerticesNum * VERTEX_C * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexSizeByte, mVerticesData, GL_STATIC_DRAW);
    glVertexAttribPointer(positionHandler, VERTEX_C, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(positionHandler);
    GL_ERROR_CHECK("BlenderModel: Update Vertex VBO");

    /*Update Normal VBO*/
    glGenBuffers(1, &mNormalPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalPositionBuffer);

    size_t normalSizeByte = mVerticesNum * NORMAL_C * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, normalSizeByte, mNormalData, GL_STATIC_DRAW);
    glVertexAttribPointer(normalHandler, NORMAL_C, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(normalHandler);
    GL_ERROR_CHECK("BlenderModel: Update Normal VBO");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    VertexGenerator::getInstance().unloadObjModel(&mVerticesData, NULL, &mNormalData);
    return true;
}

bool BlenderModel::drawPolygonEvery()
{
    glBindVertexArray(mVertexArrayObject);
    GL_ERROR_CHECK("BlenderModel:Switch VAO");

    glDrawArrays(MESH::TRIANGLES, 0, mVerticesNum);
    GL_ERROR_CHECK("BlenderModel:drawPolygon");

    glBindVertexArray(0);

    return true;
}

bool BlenderModel::updateParamsEvery()
{

    /*TODO  Model Matrix need to change every frame */



    return true;
}

}

