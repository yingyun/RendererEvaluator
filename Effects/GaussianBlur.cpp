/*
*Gaussian blur, it's a traditional method.just for fun
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/

/*
*
*Note that this filter wasn't completed yet
*/
#include "GaussianBlur.h"

namespace RenderEvaluator
{
#define VERTEC_COUNT 4
#define VERTEX_SIZE 2
#define TEXCOORDS_SIZE 2
#define Pi 3.14159265

#define SIG 1.0 //TODO: Use variable pass to replace it
#define LOOP 20

GaussianBlur::GaussianBlur(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

GaussianBlur::~GaussianBlur()
{
    /*Release source*/
    glDeleteBuffers(1, &mTextureCoordsBuffer);
    glDeleteBuffers(1, &mVertexPositionBuffer);
    glDeleteVertexArrays(1, &mVertexArrayObject);
    glDeleteTextures(1, texture);
}

/*
*Mathematical Formula
*G(x) = ( 1/sigma sqrt(2 * Pi) ) * e exp(-root(x-a) / 2 * root(signal))
*
*a = the shift from origin point forward to x-axis
*sigma = called standard deviation, root(sigma) = called variance
*Pi = 3.1415926535897932384626433832795028841971693993751
*x	= the input point from the x-axis
*input x area = in the math, we define it's nearly in (-3*sigma, +3*sigma);three times by sigma
*/

double gaussian_distribution(double x_input, double sigma, double x_shift)
{
    double front = 1.0f / (sigma * sqrt(2 * Pi));
    double back = front * exp( -(x_input * x_input) / (2 * (sigma * sigma)) );
    return back;
}

vector<double> genGaussianWeightList(double sig, int radius_pixel)
{
    double sigma = sig;
    int loop_times = radius_pixel;
    double step = (sigma * 3) / static_cast<double>(loop_times);
    double sum_of_weight = 0.0f;
    double weight;

    vector<double> distribution;
    for (int i = 0; i < loop_times; i++)
        {
            weight = gaussian_distribution( (step * i), sigma, 0);
            sum_of_weight += weight;

            distribution.push_back(weight);
        }

    sum_of_weight = sum_of_weight * 2 - distribution[0];

    double after_to_half_sum = 0.0;
    for (unsigned int i = 0; i < distribution.size(); ++i)
        {
            /*Keep the sum of one-direction was 0.5*/
            distribution[i] = distribution[i] / (sum_of_weight * 2.0);
            after_to_half_sum += distribution[i];
        }

    return distribution;
}

bool GaussianBlur::updateShaderOnce()
{
    const char * vertexShader = "\
    attribute vec4 texCoords;\n\
    varying vec2 outTexCoords;\n\
    uniform float weight0;\n\
    uniform float weight1;\n\
    uniform float weight2;\n\
    uniform float weight3;\n\
    uniform float weight4;\n\
    varying float w0;\n\
    varying float w1;\n\
    varying float w2;\n\
    varying float w3;\n\
    varying float w4;\n\
    uniform vec2 uni_step;\n\
    attribute vec4 position;\n\
    uniform mat4 projection;\n\
    void main(void)\n\
    {\n\
    	outTexCoords = texCoords.st;\n\
	w0 = weight0;\n\
	w1 = weight1;\n\
	w2 = weight2;\n\
	w3 = weight3;\n\
	w4 = weight4;\n\
	var_step = uni_step;\n\
	gl_Position = projection * position;\n\
    }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
    precision mediump float;\n\
    varying vec2 outTexCoords;\n\
    varying float w0;\n\
    varying float w1;\n\
    varying float w2;\n\
    varying float w3;\n\
    varying float w4;\n\
    varyign vec2 var_step;\n\
    uniform sampler2D sampler;\n\
    void main(void)\n\
    {\n\
	vec4 color;\n\
	color = texture2D(sampler, outTexCoords) * w0;\n\
	color += texture2D(sampler, outTexCoords - var_step * 1) * w1;\n\
	color += texture2D(sampler, outTexCoords - var_step * 2) * w2;\n\
	color += texture2D(sampler, outTexCoords - var_step * 3) * w3;\n\
	color += texture2D(sampler, outTexCoords - var_step * 4) * w4;\n\
	color += texture2D(sampler, outTexCoords + var_step * 1) * w1;\n\
	color += texture2D(sampler, outTexCoords + var_step * 2) * w2;\n\
	color += texture2D(sampler, outTexCoords + var_step * 3) * w3;\n\
	color += texture2D(sampler, outTexCoords + var_step * 4) * w4;\n\
    	gl_FragColor = color;\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);

    return true;
}

bool GaussianBlur::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    texCoordsHandler = glGetAttribLocation(mProgram, "texCoords");
    positionHandler = glGetAttribLocation(mProgram, "position");
    projectionHandler = glGetUniformLocation(mProgram, "projection");
    samplerHandler = glGetUniformLocation(mProgram, "sampler");
    w0Handler = glGetUniformLocation(mProgram, "weight0");
    w1Handler = glGetUniformLocation(mProgram, "weight1");
    w2Handler = glGetUniformLocation(mProgram, "weight2");
    w3Handler = glGetUniformLocation(mProgram, "weight3");
    w4Handler = glGetUniformLocation(mProgram, "weight4");
    stepHanlder = glGetUniformLocation(mProgram, "uni_step");
    LOG_INFO("Render=> texCoords %d, position %d, projection %d, sampler %d, w0 %d, w1 %d, w2 %d, w3 %d, w4 %d, step %d \n",
             texCoordsHandler, positionHandler, projectionHandler, samplerHandler,
             w0Handler, w1Handler, w2Handler, w3Handler, w4Handler, stepHanlder);

    float width = (float)mLayerInfo.LayerWidth;
    float height = (float)mLayerInfo.LayerHeight;

    /*Update projection matrix*/
    MatrixTransform::getInstance().matrixIndentity(&mProjectionMatrix);
    MatrixTransform::getInstance().fullScreenOrthoProj(&mProjectionMatrix, width, height);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, (GLfloat *)mProjectionMatrix.m);
    GL_ERROR_CHECK("GaussianBlur:update projection matrix");

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

    /*Update texture pixel size*/
    GL_ERROR_CHECK("GaussianBlur:update sampler and pixel size uniform");

    return true;
}

bool GaussianBlur::updateBufferOnce()
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
    GL_ERROR_CHECK("GaussianBlur:Gen texture and update image");

    /*Update VAO and vertex, texture VBO*/
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    glGenBuffers(1, &mVertexPositionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexPositionBuffer);
    int vertexByteSize = (mRectMesh.getVertexSize() + mRectMesh.getTexCoordsSize() ) * mRectMesh.getVertexCount() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexByteSize, mRectMesh.getPositions(), GL_STATIC_DRAW);
    glVertexAttribPointer(positionHandler, mRectMesh.getVertexSize(), GL_FLOAT,
                          GL_FALSE, mRectMesh.getByteStride(), (void *)0);
    glEnableVertexAttribArray(positionHandler);
    GL_ERROR_CHECK("GaussianBlur:VAO for vertex");

    glGenBuffers(1, &mTextureCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordsBuffer);
    vertexByteSize -= mRectMesh.getTexCoordsSize() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexByteSize, mRectMesh.getTexCoords(), GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsHandler, mRectMesh.getTexCoordsSize(), GL_FLOAT,
                          GL_FALSE, mRectMesh.getByteStride(), (void *)0);
    glEnableVertexAttribArray(texCoordsHandler);
    GL_ERROR_CHECK("GaussianBlur:VAO for texture");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool GaussianBlur::drawPolygonEvery()
{
    glBindVertexArray(mVertexArrayObject);
    GL_ERROR_CHECK("GaussianBlur:Switch VAO");

    glDrawArrays(MESH::TRIANGLE_FAN, 0, VERTEC_COUNT);
    GL_ERROR_CHECK("GaussianBlur:drawPolygon");

    glBindVertexArray(0);

    return true;
}

bool GaussianBlur::updateParamsEvery()
{
    return true;
}

}

