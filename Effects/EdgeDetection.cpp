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
    /* Do the shader build*/
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
    varyign vec2 outTexCoords;\n\
    varying float xPixelsize;\n\
    varying float yPixelSize;\n\
    uniform sampler2D sampler;\n\
    const vec3 lumCoeff = vec3(0.2125, 0.7154, 0.0721);\n\
    void main(void)\n\
    {\n\
    	vec2 tex = outTexCoords.st;\n\
    	vec4 texture = texture2D(sampler, tex);\n\
    	\n\
    	onCoords = vec2(-xPixelsize, yPixelSize);\n\
    	teCoords = vec2(0, yPixelSize);\n\
    	tsCoords = vec2(xPixelsize, yPixelSize);\n\
    	fsCoords = vec2(-xPixelsize, 0);\n\
          \n\
    	oneP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+onCoords)));\n\
    	twoP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+teCoords)));\n\
    	threeP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+tsCoords)));\n\
    	fourP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords+fsCoords)));\n\
          \n\
    	sixP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-fsCoords)));\n\
    	sevenP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-tsCoords)));\n\
    	eightP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-teCoords)));\n\
    	nineP = dot(lumCoeff, texture2D(sampler, vec2(outTexCoords-onCoords)));\n\
    	\n\
    	float Gx = oneP*-3 + fourP*-10 + sevenP*-3 + threeP*3 + sixP*10 + nine*3;\n\
    	float Gy = oneP*-3 + twoP*-10 + three*-3 + sevenP*3 + eightP*10 + nine*3;\n\
    	\n\
    	float mag = length(vec2(Gx, Gy));\n\
    	vec3 target = vec3(mag, mag, mag);\n\
    	gl_FragColor = vec4(mix(target, texture.rgb, 0.9), 1.0);\n\
    }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);
    GL_ERROR_CHECK;

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
             texCoordsHandler, positionHandler, projectionHandler, samplerHandler,
             xPointSizeHandler,yPointSizeHandler );



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
    GL_ERROR_CHECK;

    /*Update texture pixel size*/
    glUniform1f(xPointSizeHandler, 1.0f / mLayerInfo.LayerWidth);
    glUniform1f(yPointSizeHandler, 1.0f / mLayerInfo.LayerHeight);

    return true;
}

bool EdgeDetection::drawPolygonEvery()
{
    glDrawArrays(MESH::TRIANGLE_FAN, 0, VERTEC_COUNT);
    GL_ERROR_CHECK;

    return true;
}

bool EdgeDetection::updateFrameEvery()
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

