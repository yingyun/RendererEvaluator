/*
*Cui.YY
*The implementation of vertex geneartor.
*
*
*/


#include "VertexGenerator.h"

namespace RenderEvaluator
{
#define NUM_VERTICES 24
#define NUM_INDICES 36

/*
*This function gets from OpenGLES2.0 Programming Guide
*
*20131229 Cui.Yingyun
*Note: This generating will get the result of NDC(Normailized Coordination) verticles.
*It means that you can't use this combine with the operation of projection matrix.
*/
int VertexGenerator::generateCube(bool indexMode, float scale, float **vertices, float **normals,
                                  float **texCoords, float **colors, unsigned int **indices)
{
    if(indexMode)
        {
            //has Index
            float cubeVerts[] =
            {
                /* 24 vertex,  6 slide * 4 vertex per slide */
#if 1
                /* Full size */
                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f, 1.0f,
                -1.0f,  1.0f, 1.0f,
                1.0f,  1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
#endif

#if 0
                /* Half size */
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
#endif
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
                    *vertices = (float *)malloc(sizeof(float) * 3 * NUM_VERTICES);
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
                1.0f, 0.0f, 0.0f, .0f,
                1.0f, 0.0f, 0.0f, .0f,
                1.0f, 0.0f, 0.0f, .0f,
                1.0f, 0.0f, 0.0f, .0f,
                1.0f, 0.0f, 0.0f, .0f,
                1.0f, 0.0f, 0.0f, .0f,

                // Right face (green)
                0.0f, 1.0f, 0.0f, .0f,
                0.0f, 1.0f, 0.0f, .0f,
                0.0f, 1.0f, 0.0f, .0f,
                0.0f, 1.0f, 0.0f, .0f,
                0.0f, 1.0f, 0.0f, .0f,
                0.0f, 1.0f, 0.0f, .0f,

                // Back face (blue)
                0.0f, 0.0f, 1.0f, .0f,
                0.0f, 0.0f, 1.0f, .0f,
                0.0f, 0.0f, 1.0f, .0f,
                0.0f, 0.0f, 1.0f, .0f,
                0.0f, 0.0f, 1.0f, .0f,
                0.0f, 0.0f, 1.0f, .0f,

                // Left face (yellow)
                1.0f, 1.0f, 0.0f, .0f,
                1.0f, 1.0f, 0.0f, .0f,
                1.0f, 1.0f, 0.0f, .0f,
                1.0f, 1.0f, 0.0f, .0f,
                1.0f, 1.0f, 0.0f, .0f,
                1.0f, 1.0f, 0.0f, .0f,

                // Top face (cyan)
                0.0f, 1.0f, 1.0f, .0f,
                0.0f, 1.0f, 1.0f, .0f,
                0.0f, 1.0f, 1.0f, .0f,
                0.0f, 1.0f, 1.0f, .0f,
                0.0f, 1.0f, 1.0f, .0f,
                0.0f, 1.0f, 1.0f, .0f,

                // Bottom face (magenta)
                1.0f, 0.0f, 1.0f, .0f,
                1.0f, 0.0f, 1.0f, .0f,
                1.0f, 0.0f, 1.0f, .0f,
                1.0f, 0.0f, 1.0f, .0f,
                1.0f, 0.0f, 1.0f, .0f,
                1.0f, 0.0f, 1.0f, .0f
            };

            if(vertices != NULL)
                {
                    *vertices = (float *)malloc(sizeof(float) * 3 * NUM_INDICES);
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

unsigned int VertexGenerator::vertexCubeSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 3 * num;
}

unsigned int VertexGenerator::colorCubeSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 4 * num;

}

unsigned int VertexGenerator::texcoordCubeSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 2 * num;

}

unsigned int VertexGenerator::normalCubeSizeByte(bool indexMode)
{
    unsigned int num = 0;
    if(indexMode) num = NUM_VERTICES;
    else num = NUM_INDICES;
    return sizeof(float) * 3 * num;

}

unsigned int VertexGenerator::indexCubeSizeByte()
{
    return sizeof(unsigned int) * 1 * NUM_INDICES;
}

void VertexGenerator::generateRectangle(float width, float height, float ** vertices, float ** texCoords)
{
    generateRectangle(0.0f, height, 0.0f, 0.0f,  width, 0.0f, width, height, vertices, texCoords);
}

void VertexGenerator::generateRectangle(float p0_x, float p0_y, float p1_x, float p1_y,
                                        float p2_x, float p2_y, float p3_x, float p3_y,
                                        float ** vertices, float ** texCoords)
{
    if(vertices != NULL)
        {
            *vertices = (float *)malloc(sizeof(float) * 2 * 4);
            (*vertices)[0] = p0_x;
            (*vertices)[1] = p0_y; //left-top
            (*vertices)[2] = p1_x;
            (*vertices)[3] = p1_y; //left-bottom
            (*vertices)[4] = p2_x;
            (*vertices)[5] = p2_y; //right-bottom
            (*vertices)[6] = p3_x;
            (*vertices)[7] = p3_y; //right-top
        }
    else
        {
            printf("ERROR: Wrong pointer %s:%d", __func__, __LINE__);
        }

    if(texCoords != NULL)
        {
            *texCoords = (float *)malloc(sizeof(float) * 2 * 4);
            (*texCoords)[0] = 0.0f;
            (*texCoords)[1] = 1.0f; //left-top
            (*texCoords)[2] = 0.0f;
            (*texCoords)[3] = 0.0f; //left-bottom
            (*texCoords)[4] = 1.0f;
            (*texCoords)[5] = 0.0f ;//right-bottom
            (*texCoords)[6] = 1.0f;
            (*texCoords)[7] = 1.0f; //right-top
        }
    else
        {
            printf("ERROR: Wrong pointer %s:%d", __func__, __LINE__);
        }
}

void VertexGenerator::releaseRectangle(float ** vertices, float ** texCoords)
{
    if(vertices == 0 || texCoords == 0)
        {
            printf("ERROR: Wrong pointer %s:%d", __func__, __LINE__);
        }

    free(*vertices);
    free(*texCoords);
}

}
