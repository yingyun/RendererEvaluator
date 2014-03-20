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
}

BlenderModel::~BlenderModel()
{
    /*Release source*/
    glDeleteBuffers(1, &mVertexPositionBuffer);
    glDeleteVertexArrays(1, &mVertexArrayObject);
}

bool BlenderModel::updateShaderOnce()
{
    const char * vertexShader = "\
    attribute vec4 a_position;\n\
    uniform mat4 u_modelMatrix;\n\
    uniform mat4 u_viewMatrix;\n\
    uniform mat4 u_projection;\n\
    void main(void)\n\
    {\n\
        vec4 vertex;\n\
        vertex = u_modelMatrix * a_position;\n\
        vertex = u_viewMatrix * vertex;\n\
    	gl_Position =  u_projection * vertex;\n\
    }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
    precision mediump float;\n\
    void main(void)\n\
    {\n\
    	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);

    return true;
}

bool BlenderModel::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    positionHandler = glGetAttribLocation(mProgram, "a_position");
    modelMatrixHandler = glGetUniformLocation(mProgram, "u_modelMatrix");
    viewMatrixHandler = glGetUniformLocation(mProgram, "u_viewMatrix");
    projectionHandler = glGetUniformLocation(mProgram, "u_projection");
    LOG_INFO("Render=> a_position %d, u_modelMatrix %d, u_viewMatrix %d, u_projection %d\n",
             positionHandler, modelMatrixHandler, viewMatrixHandler, projectionHandler);

    /*Update model matrix*/
    LOG_INFO("BlenderModel: Generate model matrix\n");
    MatrixTransform::getInstance().matrixIndentity(&mModelMatrix);
    MatrixTransform::getInstance().matrixRotate(&mModelMatrix, 10.0f, 0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(modelMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mModelMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update model matrix");

    /*Update view matrix*/
    LOG_INFO("BlenderModel: Generate view matrix\n");
    MatrixTransform::getInstance().matrixIndentity(&mViewMatrix);
    MatrixTransform::getInstance().matrixLookAt(&mViewMatrix, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glUniformMatrix4fv(viewMatrixHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mViewMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update view matrix");

    /*Update projection matrix*/
    LOG_INFO("BlenderModel: Generate projection matrix\n");
    MatrixTransform::getInstance().matrixIndentity(&mProjectionMatrix);
    const float aspect = (float)mLayerInfo.LayerWidth / (float)mLayerInfo.LayerHeight;
    MatrixTransform::getInstance().matrixPerspectiveProjection(&mProjectionMatrix,
            40.0f, aspect, 1.0f, 100.0f);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, reinterpret_cast<GLfloat*>(mProjectionMatrix.m));
    GL_ERROR_CHECK("BlenderModel:update projection matrix");

    return true;
}

bool BlenderModel::updateBufferOnce()
{
    if(!VertexGenerator::getInstance().genObjectModel(mLayerInfo.LayerObjectModel, &mOBJModel))
        LOG_ERROR("BlenderModel: Generating Object model failed!");

    /*Update VAO and vertex VBO*/
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);

    size_t vertexSizeByte = mOBJModel.numberVertices * 4 * sizeof(GLfloat);

    glBufferData(GL_ARRAY_BUFFER, vertexSizeByte, reinterpret_cast<GLfloat*>(mOBJModel.vertices), GL_STATIC_DRAW);
    glVertexAttribPointer(positionHandler, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(positionHandler);
    GL_ERROR_CHECK("BlenderModel:Update VAO");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    VertexGenerator::getInstance().destroyObjectModel(&mOBJModel);

    return true;
}

bool BlenderModel::drawPolygonEvery()
{
    glBindVertexArray(mVertexArrayObject);
    GL_ERROR_CHECK("BlenderModel:Switch VAO");

    glDrawArrays(MESH::TRIANGLES, 0, mOBJModel.numberVertices);
    GL_ERROR_CHECK("BlenderModel:drawPolygon");

    glBindVertexArray(0);

    return true;
}

bool BlenderModel::updateFrameEvery()
{

    /*TODO  Model Matrix need to change every frame */



    return true;
}

}

