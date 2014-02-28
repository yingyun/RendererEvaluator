#include "GrayscaleEffect.h"

namespace RenderEvaluator
{
GrayscaleEffect::GrayscaleEffect(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

bool GrayscaleEffect::updateShaderOnce()
{
    /* Do the shader build*/
    const char * vertexShader = "\
     //GrayscaleEffect VertexShader\n\
     attribute vec4 texCoords;\n\
     varying vec2 outTexCoords;\n\
     attribute vec4 position;\n\
     uniform mat4 projection;\n\
     void main(void) {\n\
        gl_Position = projection * position;\n\
        outTexCoords = texCoords.st;\n\
    }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
    //GrayscaleEffect Fragment\n\
    precision mediump float;\n\
    varying vec2 outTexCoords;\n\
    uniform sampler2D sampler;\n\
    uniform vec4 colorOffset;\n\
    void main(void) {\n\
        vec3 lumCoeff = vec3(0.2125, 0.7154, 0.0721);\n\
        vec4 texColor = texture2D(sampler, outTexCoords);\n\
        //Do Luminance\n\
        float lum = dot(lumCoeff, texColor.rgb);\n\
        gl_FragColor = vec4(lum, lum, lum, 1.0) + colorOffset;\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);
    GL_ERROR_CHECK;

    return true;
}

bool GrayscaleEffect::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    texCoordsHandler = glGetAttribLocation(mProgram, "texCoords");
    positionHandler = glGetAttribLocation(mProgram, "position");
    projectionHandler = glGetUniformLocation(mProgram, "projection");
    samplerHandler = glGetUniformLocation(mProgram, "sampler");
    colorOffsetHandler = glGetUniformLocation(mProgram, "colorOffset");
    LOG_INFO("Render=> texCoords %d, position %d, projection %d, sampler %d, colorOffset %d\n",
             texCoordsHandler, positionHandler, projectionHandler, samplerHandler, colorOffsetHandler);

    /* Generate & Update vertex and texture coordinations */
    VertexGenerator::getInstance().generateRectangle(mLayerInfo.LayerWidth, mLayerInfo.LayerHeight, &vertexData, &texCoordsData);

    glVertexAttribPointer(texCoordsHandler, 2, GL_FLOAT, GL_FALSE, 0, texCoordsData);
    glEnableVertexAttribArray(texCoordsHandler);

    glVertexAttribPointer(positionHandler, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
    glEnableVertexAttribArray(positionHandler);

    /*Update projection matrix*/
    MatrixTransform::getInstance().matrixIndentity(&mProjectionMatrix);
    MatrixTransform::getInstance().androidStyleProjection(&mProjectionMatrix, mLayerInfo.LayerWidth, mLayerInfo.LayerHeight);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, (GLfloat *)mProjectionMatrix.m);

    /*Update sampler*/
    glUniform1i(samplerHandler, 0);

    /*Update color offset*/
    GLfloat colorOffsetData[4] = {0.1, 0.1, 0.1, 0.0};
    glUniform4fv(colorOffsetHandler, 1, colorOffsetData);
    GL_ERROR_CHECK;

    return true;
}

bool GrayscaleEffect::updateBufferOnce()
{
    /*Update Texture buffer */
    int textureWidth = 0, textureHeight = 0;
    void * pixelData;
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    TextureGenerator::getInstance().loadTexture(mLayerInfo.LayerTexture, &textureWidth, &textureHeight, &pixelData, mBitmap);
    TextureGenerator::getInstance().samplingMode(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glActiveTexture(GL_TEXTURE0);
    GL_ERROR_CHECK;

    return true;
}

bool GrayscaleEffect::drawPolygonEvery()
{
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    GL_ERROR_CHECK;

    return true;
}

bool GrayscaleEffect::updateFrameEvery()
{
    /*Update Vertex & Texture coordinations*/
    glVertexAttribPointer(texCoordsHandler, 2, GL_FLOAT, GL_FALSE, 0, texCoordsData);
    glEnableVertexAttribArray(texCoordsHandler);

    glVertexAttribPointer(positionHandler, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
    glEnableVertexAttribArray(positionHandler);
    GL_ERROR_CHECK;

    return true;
}


}
