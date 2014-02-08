/*
* 20140206 Cui.Yingyun
*/
#include "PureCanvasEffect.h"

namespace RenderEvaluator
{
PureCanvasEffect::PureCanvasEffect()
{
    /* Do something */
}

bool PureCanvasEffect::updateShaderOnce()
{
    /* Do the shader build*/
    const char * vertexShader = "\
     //PureCanvasEffect VertexShader\n\
     attribute vec4 position;\n\
     uniform mat4 projection;\n\
     void main(void) {\n\
        gl_Position = projection * position;\n\
    }\n";
    mVertexShader.append(vertexShader);
    LOG_INFO("VertexShader %s\n", mVertexShader.string());

    const char * fragShader = "\
    //PureCanvasEffect Fragment\n\
    precision mediump float;\n\
    void main(void) {\n\
        gl_FragColor = vec4(0.2, 0.3, 0.4, 1.0);\n\
    }\n";
    mFragShader.append(fragShader);
    LOG_INFO("FragmentShader %s\n", mFragShader.string());

    mProgram = ShaderProgramBuilder::buildShaderProgram(mGrayscaleVertexShader.string(), mGrayscaleFragShader.string());
    ShaderProgramBuilder::useShaderProgram(mProgram);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    positionHandler = glGetAttribLocation(mProgram, "position");
    projectionHandler = glGetUniformLocation(mProgram, "projection");
    LOG_INFO("Render=> texCoords %d, position %d, projection %d, sampler %d\n", texCoordsHandler, positionHandler,
             projectionHandler, samplerHandler);

    /* Generate & Update vertex and texture coordinations */
    VertexGenerator::generateRectangle(surfaceHeight, surfaceWidth, &vertexData, &texCoordsData);

    glVertexAttribPointer(positionHandler, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
    glEnableVertexAttribArray(positionHandler);

    /*Update projection matrix*/
    MatrixTransform::matrixIndentity(&projectionMatrix);
    MatrixTransform::androidStyleProjection(&projectionMatrix, surfaceWidth, surfaceHeight);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, (GLfloat *)projectionMatrix.m);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateBufferOnce()
{
    return true;
}

bool PureCanvasEffect::drawPolygonEvery()
{
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateFrameEvery()
{
    /*Update Vertex coordinations*/
    glVertexAttribPointer(positionHandler, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
    glEnableVertexAttribArray(positionHandler);
    GL_ERROR_CHECK;

    return true;
}


}
