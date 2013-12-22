/*
*MEIZU Technology. ZHUHAI 2013
*Cui.YY
*The implementation of vertex geneartor.
*
*
*/


#include "VertexGenerator.h"
namespace android
{
#define NUM_VERTICES 24
#define NUM_INDICES 36

/* This function gets from OpenGLES2.0 Programming Guide */
int VertexGenerator::generateCube(bool indexMode, float scale, float **vertices, float **normals,
                                  float **texCoords, float **colors, unsigned int **indices)
{
    if(indexMode)
        {
            //has Index
            float cubeVerts[] =
            {
                /* 24 vertex,  6 slide * 4 vertex per slide */
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f, -0.5f, -0.5f,

                -0.5f,  0.5f, -0.5f,
                -0.5f,  0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                0.5f,  0.5f, -0.5f,

                -0.5f, -0.5f, -0.5f,
                -0.5f,  0.5f, -0.5f,
                0.5f,  0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,

                -0.5f, -0.5f, 0.5f,
                -0.5f,  0.5f, 0.5f,
                0.5f,  0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,

                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,
                -0.5f,  0.5f,  0.5f,
                -0.5f,  0.5f, -0.5f,

                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f,  0.5f,
                0.5f,  0.5f,  0.5f,
                0.5f,  0.5f, -0.5f,
            };

            float cubeColor[] =
            {
                /* 24 vertex, 6 slide * 4 vertex per slide */
                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                1.0f, 0.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f
            };

            float cubeNormals[] =
            {
                /* 24 normals */
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 0.0f,

                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,

                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -1.0f,

                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,

                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,
                -1.0f, 0.0f, 0.0f,

                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
            };

            float cubeTex[] =
            {
                /* 24 */
                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,

                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,

                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,

                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,

                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,

                0.0f, 0.0f,
                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,
            };

            if(vertices != NULL)
                {
                    *vertices = (float *)malloc(sizeof(float) * 3 * NUM_VERTICES);//FixMe; Mem leak here
                    memcpy(*vertices, cubeVerts, sizeof(cubeVerts));
                    for(int i = 0; i < NUM_VERTICES * 3; i++)
                        {
                            (*vertices)[i] *= scale;
                        }
                }

            if(normals != NULL)
                {
                    *normals = (float *)malloc(sizeof(float) * 3 * NUM_VERTICES);
                    memcpy(*normals, cubeNormals, sizeof(cubeNormals));
                }

            if(texCoords != NULL)
                {
                    *texCoords = (float *)malloc(sizeof(float) * 2 * NUM_VERTICES);
                    memcpy(*texCoords, cubeTex, sizeof(cubeTex)) ;
                }
            if(colors != NULL)
                {
                    *colors = (float *)malloc(sizeof(float) * 4 * NUM_VERTICES);
                    memcpy(*colors, cubeColor, sizeof(cubeColor));
                }

            // Generate the indices
            if(indices != NULL)
                {
                    unsigned int cubeIndices[] =
                    {
                        0, 2, 1,
                        0, 3, 2,

                        4, 5, 6,
                        4, 6, 7,

                        8, 9, 10,
                        8, 10, 11,

                        12, 15, 14,
                        12, 14, 13,

                        16, 17, 18,
                        16, 18, 19,

                        20, 23, 22,
                        20, 22, 21
                    };

                    *indices = (unsigned int *)malloc(sizeof(unsigned int) * NUM_INDICES);
                    memcpy(*indices, cubeIndices, sizeof(cubeIndices));
                }
        }
    else //No Index
        {
            float cubeVerts[] =
            {
                // Front face
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,

                // Right face
                1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, 1.0f, -1.0f,

                // Back face
                1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                // Left face
                -1.0f, 1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,
                -1.0f, 1.0f, 1.0f,

                // Top face
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,

                // Bottom face
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, 1.0f,
                -1.0f, -1.0f, -1.0f,
            };

            float cubeColor[] =
            {
                // Front face (red)
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 0.0f, 1.0f,

                // Right face (green)
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,
                0.0f, 1.0f, 0.0f, 1.0f,

                // Back face (blue)
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,

                // Left face (yellow)
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f,

                // Top face (cyan)
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 1.0f, 1.0f,

                // Bottom face (magenta)
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, 0.0f, 1.0f, 1.0f
            };

            // TODO: implement texcoord and normal

            if(vertices != NULL)
                {
                    *vertices = (float *)malloc(sizeof(float) * 3 * NUM_INDICES);//FixMe; Mem leak here
                    memcpy(*vertices, cubeVerts, sizeof(cubeVerts));
                    for(int i = 0; i < NUM_INDICES * 3; i++)
                        {
                            (*vertices)[i] *= scale;
                        }
                }

            if(colors != NULL)
                {
                    *colors = (float *)malloc(sizeof(float) * 4 * NUM_INDICES);
                    memcpy(*colors, cubeColor, sizeof(cubeColor));
                }
        }
    return NUM_INDICES;
}

void VertexGenerator::releaseCube(float **vertices, float **normals,
                                  float **texCoords, float **colors, unsigned int **indices)
{
    if(vertices != NULL)
        {
            free(*vertices);
            *vertices = 0;
        }
    if(normals != NULL)
        {
            free(*normals);
            *normals = 0;
        }
    if(texCoords != NULL)
        {
            free(*texCoords);
            *texCoords = 0;
        }
    if(colors != NULL)
        {
            free(*colors);
            *colors = 0;
        }
    if(indices != NULL)
        {
            free(*indices);
            *indices = 0;
        }
}

unsigned int VertexGenerator::vertexSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 3 * num;
}

unsigned int VertexGenerator::colorSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 4 * num;

}

unsigned int VertexGenerator::texcoordSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 2 * num;

}

unsigned int VertexGenerator::normalSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 3 * num;

}

unsigned int VertexGenerator::indexSizeByte()
{
    return sizeof(unsigned int) * 1 * NUM_INDICES;
}

}
