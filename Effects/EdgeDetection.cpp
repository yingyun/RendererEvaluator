#include "EdgeDetection.h"

namespace RenderEvaluator
{
#define VERTEC_COUNT 4
#define VERTEX_SIZE 2
#define TEXCOORDS_SIZE 2

EdgeDetection::EdgeDetection(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

bool EdgeDetection::updateShaderOnce()
{
    const char * vertexShader = "\
    attribute vec4 texCoords;\n\
    varying vec2 outTexCoords;\n\
    varying float xPixelsize;\n\
    varying float yPixelSize;\n\
    uniform float xPS;\n\
    uniform float yPS;\n\
    attribute vec4 position;\n\
    uniform mat4 projection;\n\
    void main(void)\n\
    {\n\
    	gl_Position = projection * position;\n\
    	outTexCoords = texCoords.st;\n\
    	xPixelsize = xPS;\n\
    	yPixelSize = yPS;\n\
    }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
    precision mediump float;\n\
    varying vec2 outTexCoords;\n\
    varying float xPixelsize;\n\
    varying float yPixelSize;\n\
    uniform sampler2D sampler;\n\
    const vec3 lumCoeff = vec3(0.2125, 0.7154, 0.0721);\n\
    void main(void)\n\
    {\n\
    	vec2 tex = outTexCoords.st;\n\
    	vec4 texture = texture2D(sampler, tex);\n\
    	\n\
    	vec2 onCoords = vec2(-xPixelsize, yPixelSize);\n\
    	vec2 teCoords = vec2(0, yPixelSize);\n\
    	vec2 tsCoords = vec2(xPixelsize, yPixelSize);\n\
    	vec2 fsCoords = vec2(-xPixelsize, 0);\n\
        \n\
    	float oneP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+onCoords)).rgb);\n\
    	float twoP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+teCoords)).rgb);\n\
    	float threeP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+tsCoords)).rgb);\n\
    	float fourP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+fsCoords)).rgb);\n\
        \n\
    	float sixP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-fsCoords)).rgb);\n\
    	float sevenP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-tsCoords)).rgb);\n\
    	float eightP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-teCoords)).rgb);\n\
    	float nineP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-onCoords)).rgb);\n\
    	\n\
    	float Gx = oneP*-2.0f + fourP*-8.0f + sevenP*-2.0f + threeP*2.0f + sixP*8.0f + nineP*2.0f;\n\
    	float Gy = oneP*-2.0f + twoP*-8.0f + threeP*-2.0f + sevenP*2.0f + eightP*8.0f + nineP*2.0f;\n\
    	\n\
    	float edgeDetection = length(vec2(Gx, Gy));\n\
    	vec3 target = vec3(edgeDetection - 0.5f, edgeDetection, edgeDetection);\n\
    	gl_FragColor = vec4(mix(target, texture.rgb, 0.25), 1.0);\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);

    return true;
}

bool EdgeDetection::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    texCoordsHandler = glGetAttribLocation(mProgram, "texCoords");
    positionHandler = glGetAttribLocation(mProgram, "position");
    projectionHandler = glGetUniformLocation(mProgram, "projection");
    samplerHandler = glGetUniformLocation(mProgram, "sampler");
    xPointSizeHandler = glGetUniformLocation(mProgram, "xPS");
    yPointSizeHandler = glGetUniformLocation(mProgram, "yPS");
    LOG_INFO("Render=> texCoords %d, position %d, projection %d, sampler %d, xPS %d, yPS %d\n",
             texCoordsHandler, positionHandler, projectionHandler, samplerHandler, xPointSizeHandler,yPointSizeHandler);

    float width = (float)mLayerInfo.LayerWidth;
    float height = (float)mLayerInfo.LayerHeight;

    /*Update projection matrix*/
    MatrixTransform::getInstance().matrixIndentity(&mProjectionMatrix);
    MatrixTransform::getInstance().androidStyleProjection(&mProjectionMatrix, width, height);
    glUniformMatrix4fv(projectionHandler, 1, GL_FALSE, (GLfloat *)mProjectionMatrix.m);
    GL_ERROR_CHECK("EdgeDetection:update projection matrix");

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
    glUniform1f(xPointSizeHandler, 1.0f / width);
    glUniform1f(yPointSizeHandler, 1.0f / height);
    GL_ERROR_CHECK("EdgeDetection:update sampler and pixel size uniform");

    return true;
}

bool EdgeDetection::updateBufferOnce()
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
    GL_ERROR_CHECK("EdgeDetection:Gen texture and update image");

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
    GL_ERROR_CHECK("EdgeDetection:VAO for vertex");

    glGenBuffers(1, &mTextureCoordsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordsBuffer);
    vertexByteSize -= mRectMesh.getTexCoordsSize() * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, vertexByteSize, mRectMesh.getTexCoords(), GL_STATIC_DRAW);
    glVertexAttribPointer(texCoordsHandler, mRectMesh.getTexCoordsSize(), GL_FLOAT,
        GL_FALSE, mRectMesh.getByteStride(), (void *)0);
    glEnableVertexAttribArray(texCoordsHandler);
    GL_ERROR_CHECK("EdgeDetection:VAO for texture");

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool EdgeDetection::drawPolygonEvery()
{
    glBindVertexArray(mVertexArrayObject);
    GL_ERROR_CHECK("EdgeDetection:Switch VAO");

    glDrawArrays(MESH::TRIANGLE_FAN, 0, VERTEC_COUNT);
    GL_ERROR_CHECK("EdgeDetection:drawPolygon");

    glBindVertexArray(0);

    return true;
}

bool EdgeDetection::updateFrameEvery()
{
    return true;
}

}

