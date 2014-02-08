/*
*  201231228 Cui.Yingyun
*
*   Used to generate vertex, e.g. Cube, read Blender output...
*/

#ifndef  VERTEXGENERATOR_H
#define VERTEXGENERATOR_H

#include <stdio.h>
#include <stdlib.h>

namespace RenderEvaluator
{

class VertexGenerator
{
public:
    /*
     *Generate NDC based verticle which just used without projection matrix.
     *indexMode: index mode or not
     *scale: scale the polygon
     *vertices: the result of vertices
     *normals: the result of normals
     *texCoords: the result of texture coordination
     *colors: the result of cube color which combine use without texture coordination
     *indices: the result of indices if indexMode was true
     *
     *return: the numbur of vertices was created.
     */
    static int generateCube(bool indexMode, float scale, float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);
    static void releaseCube(float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);
    /*
     * Get the attribute of Cube
     */
    static unsigned int vertexCubeSizeByte(bool indexMode);
    static unsigned int colorCubeSizeByte(bool indexMode);
    static unsigned int texcoordCubeSizeByte(bool indexMode);
    static unsigned int normalCubeSizeByte(bool indexMode);
    static unsigned int indexCubeSizeByte();

    /*
     *The point start count from left-top to left-bottom and right-bottom then right-top.
     *Just counter-clock sequence.
     *The left-bottom point was the origin(0.0f, 0.0f)
     * Note that it's based on screen coordination
     *
     */
    static void generateRectangle(float width, float height, float ** vertices, float ** texCoords);

    static void generateRectangle(float p0_x, float p0_y, float p1_x, float p1_y,
                                  float p2_x, float p2_y, float p3_x, float p3_y,
                                  float **vertices, float **texCoords);
    static void releaseRectangle(float **vertices, float **texCoords);
};

}

#endif
