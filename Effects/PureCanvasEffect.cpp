#include "PureCanvasEffect.h"

namespace RenderEvaluator
{
#define VERTEC_COUNT 4
#define VERTEX_SIZE 2
#define TEXCOORDS_SIZE 2

PureCanvasEffect::PureCanvasEffect(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

bool PureCanvasEffect::updateShaderOnce()
{
    /* Do the shader build*/
    const char * vertexShader = "\
     //PureCanvasEffect VertexShader\n\
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
    //PureCanvasEffect Fragment\n\
    precision mediump float;\n\
    varying vec2 outTexCoords;\n\
    uniform sampler2D sampler;\n\
    void main(void) {\n\
        gl_FragColor = texture2D(sampler, outTexCoords);\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    texCoordsHandler = glGetAttribLocation(mProgram, "texCoords");
    positionHandler = glGetAttribLocation(mProgram, "position");
    projectionHandler = glGetUniformLocation(mProgram, "projection");
    samplerHandler = glGetUniformLocation(mProgram, "sampler");
    LOG_INFO("Render=> texCoords %d, position %d, projection %d, sampler %d\n",
             texCoordsHandler, positionHandler, projectionHandler, samplerHandler);



    float width = (float)mLayerInfo.LayerWidth;
    float height = (float)mLayerInfo.LayerHeight;

    /*Update projection matrix*/
    MatrixTransform::getInstance().matrixIndentity(&mProjectionMatrix);
    MatrixTransform::getInstance().androidStyleProjection(&mProjectionMatrix, width, height);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, (GLfloat *)mProjectionMatrix.m);


    /* Generate & Update vertex and texture coordinations */
    MESH mesh(VertexGenerator::Mesh2D::TRIANGLE_FAN, 
    VERTEC_COUNT, VERTEX_SIZE, TEXCOORDS_SIZE);
    MESH::VertexArray<vec2f> position(mesh.getPositionArray<vec2f>());
    MESH::VertexArray<vec2f> texCoord(mesh.getTexCoordArray<vec2f>());
    position[0] = vec2f(0.0, 0.0);
    position[1] = vec2f(width, 0.0);
    position[2] = vec2f(width, height);
    position[3] = vec2f(0.0, height);

    texCoord[0] = vec2f(0.0, 0.0);
    texCoord[1] = vec2f(1.0, 0.0);
    texCoord[2] = vec2f(1.0, 1.0);
    texCoord[3] = vec2f(0.0, 1.0);

    mRectMesh = mesh;
    mRectMesh.dumpInfo();
    
    /*Update sampler*/
    glUniform1i(samplerHandler, 0);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateBufferOnce()
{
    /*Update Texture buffer */
    int textureWidth = 0, textureHeight = 0;
    void * pixelData;
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    TextureGenerator::getInstance().loadTexture(mLayerInfo.LayerTexture, 
        &textureWidth, &textureHeight, &pixelData, mBitmap);
    TextureGenerator::getInstance().samplingMode(GL_NEAREST, GL_NEAREST, 
        GL_REPEAT, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    glActiveTexture(GL_TEXTURE0);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::drawPolygonEvery()
{
    glDrawArrays(MESH::TRIANGLE_FAN, 0, VERTEC_COUNT);
    GL_ERROR_CHECK;

    return true;
}

bool PureCanvasEffect::updateFrameEvery()
{
	/*Update Vertex & Texture coordinations*/
    glVertexAttribPointer(positionHandler, mRectMesh.getVertexSize(), GL_FLOAT, 
    GL_FALSE, mRectMesh.getByteStride(), mRectMesh.getPositions());
    glEnableVertexAttribArray(positionHandler);

    glVertexAttribPointer(texCoordsHandler, mRectMesh.getTexCoordsSize(), GL_FLOAT, 
    GL_FALSE, mRectMesh.getByteStride(), mRectMesh.getTexCoords());
    glEnableVertexAttribArray(texCoordsHandler);
    GL_ERROR_CHECK;

    return true;
}

}

