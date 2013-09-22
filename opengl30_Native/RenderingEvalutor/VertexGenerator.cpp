/*
* Implementation of generator machine
* Todo:
* 1: Cube
* 2: Blender
*
*/

#include "VertexGenerator.h"
namespace android
{
/* This function gets from OpenGLES2.0 Programming Guide */
int VertexGenerator::generateCube(float scale, float **vertices, float **normals,
                                  float **texCoords, float **colors, unsigned int **indices)
{
    int i;
    int numVertices = 24;
    int numIndices = 36;
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
        1.0f, 0.0f, 0.0f, 1.0f,//Red
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 0.0f, 1.0f,//Green
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, 1.0f,//Blue
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,

        1.0f, 1.0f, 0.0f, 1.0f,//Yellow
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,

        0.0f, 1.0f, 1.0f, 1.0f,//Cyan
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 1.0f, 1.0f,//Magenta
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
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

    // Allocate memory for buffers
    if ( vertices != NULL )
        {
            /* FixMe; Maybe leak here */
            *vertices = (float *)malloc( sizeof(float) * 3 * numVertices );
            memcpy( *vertices, cubeVerts, sizeof( cubeVerts ) );
            for ( i = 0; i < numVertices * 3; i++ )
                {
                    (*vertices)[i] *= scale;
                }
        }

    if ( normals != NULL )
        {
            *normals = (float *)malloc( sizeof(float) * 3 * numVertices );
            memcpy( *normals, cubeNormals, sizeof( cubeNormals ) );
        }

    if ( texCoords != NULL )
        {
            *texCoords = (float *)malloc( sizeof(float) * 2 * numVertices );
            memcpy( *texCoords, cubeTex, sizeof( cubeTex ) ) ;
        }

    // Generate the indices
    if ( indices != NULL )
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

            *indices = (unsigned int *)malloc( sizeof(unsigned int) * numIndices );
            memcpy( *indices, cubeIndices, sizeof( cubeIndices ) );
        }

    return numIndices;
}
}
