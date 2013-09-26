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
    static int generateCube(bool indexMode, float scale, float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);
    static void releaseCube(float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);


    /*
    *   Get the byte of each vertex attribute
    */
    static unsigned int vertexSizeByte(bool indexMode);
    static unsigned int colorSizeByte(bool indexMode);
    static unsigned int texcoordSizeByte(bool indexMode);
    static unsigned int normalSizeByte(bool indexMode);
    static unsigned int indexSizeByte();


};

}

#endif
