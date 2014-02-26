#ifndef  VERTEXGENERATOR_H
#define VERTEXGENERATOR_H

#include <stdio.h>
#include <stdlib.h>

#include "Pattern/Singleton.h"

namespace RenderEvaluator
{

class VertexGenerator : public Singleton<VertexGenerator>
{
public:
    VertexGenerator() {}
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
    int generateCube(bool indexMode, float scale, float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);
    void releaseCube(float **vertices, float **normals,
                            float **texCoords, float **colors, unsigned int **indices);
    /*
     * Get the attribute of Cube
     */
    unsigned int vertexCubeSizeByte(bool indexMode);
    unsigned int colorCubeSizeByte(bool indexMode);
    unsigned int texcoordCubeSizeByte(bool indexMode);
    unsigned int normalCubeSizeByte(bool indexMode);
    unsigned int indexCubeSizeByte();

    /*
     *The point start count from left-top to left-bottom and right-bottom then right-top.
     *Just counter-clock sequence.
     *The left-bottom point was the origin(0.0f, 0.0f)
     * Note that it's based on screen coordination
     *
     */
    void generateRectangle(float width, float height, float ** vertices, float ** texCoords);

    void generateRectangle(float p0_x, float p0_y, float p1_x, float p1_y,
                                  float p2_x, float p2_y, float p3_x, float p3_y,
                                  float **vertices, float **texCoords);
    void releaseRectangle(float **vertices, float **texCoords);
};

}

#endif
