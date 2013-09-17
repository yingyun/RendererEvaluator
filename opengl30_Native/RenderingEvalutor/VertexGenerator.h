/*
*   Used to generate vertex, e.g. Cube, read Blender output...
*
*
*
*
*/

#ifndef  VERTEXGENERATOR_H
#define VERTEXGENERATOR_H

namespace android
{

#include <stdio.h>
#include <stdlib.h>
class VertexGenerator
{
public:
    static int generateCube(float scale, float **vertices, float **normals,
                                      float **texCoords, unsigned int **indices);
};

}

#endif
