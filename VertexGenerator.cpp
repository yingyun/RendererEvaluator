/*
*polygon generater.
*Object loader code get from GLUS project*
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/

#include "VertexGenerator.h"

namespace RenderEvaluator
{

typedef VertexGenerator::Mesh2D MESH2D;

RENDEREVALUATOR_SINGLETON_STATIC_VAR_INIT(VertexGenerator);

/*------------------------MESH toolset------------------------*/
VertexGenerator::Mesh2D& VertexGenerator::Mesh2D::operator = (VertexGenerator::Mesh2D& mesh)
{
    mPrimitive = mesh.getPrimitive();
    mVertexCount = mesh.getVertexCount();
    mVertexSize = mesh.getVertexSize();
    mTexCoordsSize = mesh.getTexCoordsSize();
    mStride = mesh.getStride();
    mVertices = 0;
    float* sourceVertices = mesh.getPositions();
    unsigned int byteOfmVertices = (mVertexSize + mTexCoordsSize) * mVertexCount;
    mVertices = new float[byteOfmVertices];
    if(mVertices != 0)
        {
            for(unsigned int i =0; i < byteOfmVertices; i++)
                mVertices[i] = sourceVertices[i];
        }
    else
        {
            LOG_ERROR("Wrong Mesh2D init\n");
            exit(-1);
        }
    return *this;
}

VertexGenerator::Mesh2D::Mesh2D(Primitive primitive, size_t vertexCount,
                                size_t vertexSize, size_t texCoordsSize):
    mPrimitive(primitive),
    mVertexCount(vertexCount),
    mVertexSize(vertexSize),
    mTexCoordsSize(texCoordsSize)
{
    mVertices = new float[(mVertexSize + mTexCoordsSize) * mVertexCount];
    mStride = mVertexSize + mTexCoordsSize;
}

VertexGenerator::Mesh2D::~Mesh2D()
{
    delete [] mVertices;
}

void VertexGenerator::Mesh2D::dumpInfo()
{
    LOG_INFO("<-Dumpping Mesh2D information->\n");
    LOG_INFO("Vertex Point Count: %d, Vertex Size: %d, TexCoords Size: %d, Byte Stride: %d\n",
             getVertexCount(), getVertexSize(), getTexCoordsSize(), getByteStride());
    for(size_t i = 0; i < getVertexCount(); ++i)
        {
            float* position = getPositions() + getStride() * i;
            LOG_INFO("VertexPosition[%d]: %f, %f\n", i, *position, *(position + 1));
        }
    for(size_t i = 0; i < getVertexCount(); ++i)
        {
            float* texCoords = getTexCoords() + getStride() * i;
            LOG_INFO("TexCoords[%d]: %f, %f\n", i, *texCoords, *(texCoords + 1));
        }
}

/*------------------------Linear Algebra toolset------------------------*/
float  VertexGenerator::OMVector3Lengthf(const float vector[3])
{
    return sqrtf(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}

void VertexGenerator::OMPoint4SubtractPoint4f(float result[3], const float point0[4], const float point1[4])
{
    result[0] = point0[0] - point1[0];
    result[1] = point0[1] - point1[1];
    result[2] = point0[2] - point1[2];
}

bool VertexGenerator::OMVector3Normalizef(float vector[3])
{
    int i;

    float length = OMVector3Lengthf(vector);

    if (length == 0.0f)
        {
            return false;
        }

    for (i = 0; i < 3; i++)
        {
            vector[i] /= length;
        }

    return true;
}

void VertexGenerator::OMVector3Crossf(float result[3], const float vector0[3], const float vector1[3])
{
    int i;

    float temp[3];

    temp[0] = vector0[1] * vector1[2] - vector0[2] * vector1[1];
    temp[1] = vector0[2] * vector1[0] - vector0[0] * vector1[2];
    temp[2] = vector0[0] * vector1[1] - vector0[1] * vector1[0];

    for (i = 0; i < 3; i++)
        {
            result[i] = temp[i];
        }
}

bool VertexGenerator::OMCalculateTangentSpacef(OBJModel* shape)
{
    unsigned int i;

    if (!shape || !shape->vertices || !shape->texCoords || shape->mode != GL_TRIANGLES)
        {
            return false;
        }

    // Allocate memory if needed
    if (!shape->tangents)
        {
            shape->tangents = (float*) malloc(3 * shape->numberVertices * sizeof(float));

            if (!shape->tangents)
                {
                    return false;
                }
        }

    if (!shape->bitangents)
        {
            shape->bitangents = (float*) malloc(3 * shape->numberVertices * sizeof(float));

            if (!shape->bitangents)
                {
                    return false;
                }
        }

    // Reset all tangents to 0.0f
    for (i = 0; i < shape->numberVertices; i++)
        {
            shape->tangents[i * 3] = 0.0f;
            shape->tangents[i * 3 + 1] = 0.0f;
            shape->tangents[i * 3 + 2] = 0.0f;
        }

    if (shape->numberIndices > 0)
        {
            float s1, t1, s2, t2;
            float Q1[4];
            float Q2[4];
            float tangent[3];
            float scalar;

            for (i = 0; i < shape->numberIndices; i += 3)
                {
                    s1 = shape->texCoords[2*shape->indices[i+1]] - shape->texCoords[2*shape->indices[i]];
                    t1 = shape->texCoords[2*shape->indices[i+1]+1] - shape->texCoords[2*shape->indices[i]+1];
                    s2 = shape->texCoords[2*shape->indices[i+2]] - shape->texCoords[2*shape->indices[i]];
                    t2 = shape->texCoords[2*shape->indices[i+2]+1] - shape->texCoords[2*shape->indices[i]+1];

                    scalar = 1.0f / (s1*t2-s2*t1);

                    OMPoint4SubtractPoint4f(Q1, &shape->vertices[4*shape->indices[i+1]], &shape->vertices[4*shape->indices[i]]);
                    Q1[3] = 1.0f;
                    OMPoint4SubtractPoint4f(Q2, &shape->vertices[4*shape->indices[i+2]], &shape->vertices[4*shape->indices[i]]);
                    Q2[3] = 1.0f;

                    tangent[0] = scalar * (t2 * Q1[0] - t1 * Q2[0]);
                    tangent[1] = scalar * (t2 * Q1[1] - t1 * Q2[1]);
                    tangent[2] = scalar * (t2 * Q1[2] - t1 * Q2[2]);

                    OMVector3Normalizef(tangent);

                    shape->tangents[3 * shape->indices[i]] += tangent[0];
                    shape->tangents[3 * shape->indices[i] + 1] += tangent[1];
                    shape->tangents[3 * shape->indices[i] + 2] += tangent[2];

                    shape->tangents[3 * shape->indices[i+1]] += tangent[0];
                    shape->tangents[3 * shape->indices[i+1] + 1] += tangent[1];
                    shape->tangents[3 * shape->indices[i+1] + 2] += tangent[2];

                    shape->tangents[3 * shape->indices[i+2]] += tangent[0];
                    shape->tangents[3 * shape->indices[i+2] + 1] += tangent[1];
                    shape->tangents[3 * shape->indices[i+2] + 2] += tangent[2];
                }
        }
    else
        {
            float s1, t1, s2, t2;
            float Q1[4];
            float Q2[4];
            float tangent[3];
            float scalar;

            for (i = 0; i < shape->numberVertices; i += 3)
                {
                    s1 = shape->texCoords[2*(i+1)] - shape->texCoords[2*i];
                    t1 = shape->texCoords[2*(i+1)+1] - shape->texCoords[2*i+1];
                    s2 = shape->texCoords[2*(i+2)] - shape->texCoords[2*i];
                    t2 = shape->texCoords[2*(i+2)+1] - shape->texCoords[2*i+1];

                    scalar = 1.0f / (s1*t2-s2*t1);

                    OMPoint4SubtractPoint4f(Q1, &shape->vertices[4*(i+1)], &shape->vertices[4*i]);
                    Q1[3] = 1.0f;
                    OMPoint4SubtractPoint4f(Q2, &shape->vertices[4*(i+2)], &shape->vertices[4*i]);
                    Q2[3] = 1.0f;

                    tangent[0] = scalar * (t2 * Q1[0] - t1 * Q2[0]);
                    tangent[1] = scalar * (t2 * Q1[1] - t1 * Q2[1]);
                    tangent[2] = scalar * (t2 * Q1[2] - t1 * Q2[2]);

                    OMVector3Normalizef(tangent);

                    shape->tangents[3 * i] += tangent[0];
                    shape->tangents[3 * i + 1] += tangent[1];
                    shape->tangents[3 * i + 2] += tangent[2];

                    shape->tangents[3 * (i+1)] += tangent[0];
                    shape->tangents[3 * (i+1) + 1] += tangent[1];
                    shape->tangents[3 * (i+1) + 2] += tangent[2];

                    shape->tangents[3 * (i+2)] += tangent[0];
                    shape->tangents[3 * (i+2) + 1] += tangent[1];
                    shape->tangents[3 * (i+2) + 2] += tangent[2];
                }
        }

    // Normalize, as several triangles have added a vector
    for (i = 0; i < shape->numberVertices; i++)
        {
            OMVector3Normalizef(&(shape->tangents[i * 3]));
        }

    // Calculate bitangents out of tangents and normals
    for (i = 0; i < shape->numberVertices; i++)
        {
            OMVector3Crossf(&(shape->bitangents[i * 3]), &(shape->normals[i * 3]), &(shape->tangents[i * 3]));
        }

    return true;
}

/*
        float* vertices;
        float* normals;
        float* tangents;
        float* bitangents;
        float* texCoords;
        float* allAttributes;
        unsigned short* indices;
        unsigned short numberVertices;
        unsigned int numberIndices;
        unsigned int mode;
*/

/*------------------------Object model toolset------------------------*/
bool VertexGenerator::genObjectModel(string objName, OBJModel* shape)
{
    string  filleName = string(OBJ_MODEL_PATH) + string(objName);
    bool result = OMParseObjFile(filleName, shape, 0);
    LOG_INFO("ObjModel: numVertices %d, numIndices %d, vertex=%p, normal=%p, texCoords=%p \n",
             shape->numberVertices, shape->numberIndices, shape->vertices, shape->normals, shape->texCoords);
    return result;
}

/*
* OBJ (or .OBJ) is a geometry definition file format first developed by Wavefront Technologies
* for its Advanced Visualizer animation package. The file format is open and has been adopted
* by other 3D graphics application vendors. For the most part it is a universally accepted format.
*
* Reference: http://en.wikipedia.org/wiki/Wavefront_.obj_file
*/
bool VertexGenerator::OMParseObjFile(string filePath, OBJModel* shape, OMwavefront* wavefront)
{
    bool result;
    const char* filename = filePath.data();
    FILE* f;

    char buffer[OM_BUFFERSIZE];
    char identifier[7];

    float x, y, z;
    float s, t;

    float* vertices = 0;
    float* normals = 0;
    float* texCoords = 0;

    unsigned int numberVertices = 0;
    unsigned int numberNormals = 0;
    unsigned int numberTexCoords = 0;

    float* triangleVertices = 0;
    float* triangleNormals = 0;
    float* triangleTexCoords = 0;

    unsigned int totalNumberVertices = 0;
    unsigned int totalNumberNormals = 0;
    unsigned int totalNumberTexCoords = 0;

    unsigned int facesEncoding = 0;

    char name[OM_MAX_STRING];

    unsigned int numberIndicesGroup = 0;
    unsigned int numberMaterials = 0;
    unsigned int numberGroups = 0;

    OMgroupList* currentGroupList = 0;

    if (!filename || !shape)
        {
            return false;
        }

    memset(shape, 0, sizeof(OBJModel));
    LOG_INFO("ObjModel: Opening %s!\n", filename);

    f = fopen(filename, "r");

    if (!f)
        {
            LOG_ERROR("ObjModel: Open target obj file failed!\n");
            return false;
        }

    if (!OMMallocTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                            &triangleTexCoords))
        {
            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                             &triangleTexCoords);

            fclose(f);

            return false;
        }

    while (!feof(f))
        {
            if (fgets(buffer, OM_BUFFERSIZE, f) == 0)
                {
                    if (ferror(f))
                        {
                            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                             &triangleTexCoords);

                            fclose(f);

                            return false;
                        }
                }

            if (wavefront)
                {
                    if (strncmp(buffer, "mtllib", 6) == 0)
                        {
                            sscanf(buffer, "%s %s", identifier, name);

                            if (numberMaterials == 0)
                                {
                                    wavefront->materials = 0;
                                }

                            if (!OMLoadMaterial(name, &wavefront->materials))
                                {
                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                                     &triangleTexCoords);

                                    fclose(f);

                                    return false;
                                }

                            numberMaterials++;
                        }
                    else if (strncmp(buffer, "usemtl", 6) == 0)
                        {
                            if (!currentGroupList)
                                {
                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                                     &triangleTexCoords);

                                    fclose(f);

                                    return false;
                                }

                            sscanf(buffer, "%s %s", identifier, name);

                            strcpy(currentGroupList->group.materialName, name);
                        }
                    else if (strncmp(buffer, "g", 1) == 0)
                        {
                            OMgroupList* newGroupList;

                            sscanf(buffer, "%s %s", identifier, name);

                            newGroupList = (OMgroupList*)malloc(sizeof(OMgroupList));

                            if (!newGroupList)
                                {
                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                                     &triangleTexCoords);

                                    fclose(f);

                                    return false;
                                }

                            memset(newGroupList, 0, sizeof(OMgroupList));

                            strcpy(newGroupList->group.name, name);

                            if (numberGroups == 0)
                                {
                                    wavefront->groups = newGroupList;
                                }
                            else
                                {
                                    if (!currentGroupList)
                                        {
                                            free(newGroupList);

                                            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                                             &triangleTexCoords);

                                            fclose(f);

                                            return false;
                                        }

                                    currentGroupList->next = newGroupList;

                                    currentGroupList->group.numberIndices = numberIndicesGroup;
                                    numberIndicesGroup = 0;
                                }

                            currentGroupList = newGroupList;

                            numberGroups++;
                        }
                }

            //Vertex Texture
            if (strncmp(buffer, "vt", 2) == 0)
                {
                    if (numberTexCoords == OM_MAX_ATTRIBUTES)
                        {
                            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                             &triangleTexCoords);

                            fclose(f);

                            return false;
                        }

                    sscanf(buffer, "%s %f %f", identifier, &s, &t);
                    LOG_DEBUG("ObjModel: VertexTexture: s %f, t %f\n", s, t);

                    texCoords[2 * numberTexCoords + 0] = s;
                    texCoords[2 * numberTexCoords + 1] = t;

                    numberTexCoords++;
                }
            //Vertex Normal
            else if (strncmp(buffer, "vn", 2) == 0)
                {
                    if (numberNormals == OM_MAX_ATTRIBUTES)
                        {
                            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                             &triangleTexCoords);

                            fclose(f);

                            return false;
                        }

                    sscanf(buffer, "%s %f %f %f", identifier, &x, &y, &z);
                    LOG_DEBUG("ObjModel: %s VertexNormal: x %f, y %f, z %f\n", identifier, x, y, z);

                    normals[3 * numberNormals + 0] = x;
                    normals[3 * numberNormals + 1] = y;
                    normals[3 * numberNormals + 2] = z;

                    numberNormals++;
                }
            //Vertex
            else if (strncmp(buffer, "v", 1) == 0)
                {
                    if (numberVertices == OM_MAX_ATTRIBUTES)
                        {
                            OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals,
                                             &triangleTexCoords);

                            fclose(f);

                            return false;
                        }

                    sscanf(buffer, "%s %f %f %f", identifier, &x, &y, &z);
                    LOG_DEBUG("ObjModel: %s Vertex: x %f, y %f, z %f\n", identifier, x, y, z);

                    vertices[4 * numberVertices + 0] = x;
                    vertices[4 * numberVertices + 1] = y;
                    vertices[4 * numberVertices + 2] = z;
                    vertices[4 * numberVertices + 3] = 1.0f;

                    numberVertices++;
                }
            //Face, index list
            else if (strncmp(buffer, "f", 1) == 0)
                {
                    char* token;

                    int vIndex, vtIndex, vnIndex;

                    unsigned int edgeCount = 0;

                    token = strtok(buffer, " \t");
                    token = strtok(0, " \n");

                    if (!token)
                        {
                            continue;
                        }

                    // Check faces
                    if (strstr(token, "//") != 0)
                        {
                            facesEncoding = 2;
                        }
                    else if (strstr(token, "/") == 0)
                        {
                            facesEncoding = 0;
                        }
                    else if (strstr(token, "/") != 0)
                        {
                            char* c = strstr(token, "/");

                            c++;

                            if (!c)
                                {
                                    continue;
                                }

                            if (strstr(c, "/") == 0)
                                {
                                    facesEncoding = 1;
                                }
                            else
                                {
                                    facesEncoding = 3;
                                }
                        }

                    while (token != 0)
                        {
                            vIndex = -1;
                            vtIndex = -1;
                            vnIndex = -1;

                            switch (facesEncoding)
                                {
                                    case 0:
                                        sscanf(token, "%d", &vIndex);
                                        break;
                                    case 1:
                                        sscanf(token, "%d/%d", &vIndex, &vtIndex);
                                        break;
                                    case 2:
                                        sscanf(token, "%d//%d", &vIndex, &vnIndex);
                                        break;
                                    case 3:
                                        sscanf(token, "%d/%d/%d", &vIndex, &vtIndex, &vnIndex);
                                        break;
                                }

                            vIndex--;
                            vtIndex--;
                            vnIndex--;

                            if (vIndex >= 0)
                                {
                                    if (edgeCount < 3)
                                        {
                                            if (totalNumberVertices >= OM_MAX_TRIANGLE_ATTRIBUTES)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleVertices[4 * totalNumberVertices], &vertices[4 * vIndex], 4 * sizeof(float));

                                            totalNumberVertices++;
                                            numberIndicesGroup++;
                                        }
                                    else
                                        {
                                            if (totalNumberVertices >= OM_MAX_TRIANGLE_ATTRIBUTES - 2)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleVertices[4 * (totalNumberVertices)],
                                                   &triangleVertices[4 * (totalNumberVertices - edgeCount)], 4 * sizeof(float));
                                            memcpy(&triangleVertices[4 * (totalNumberVertices + 1)],
                                                   &triangleVertices[4 * (totalNumberVertices - 1)], 4 * sizeof(float));
                                            memcpy(&triangleVertices[4 * (totalNumberVertices + 2)],
                                                   &vertices[4 * vIndex], 4 * sizeof(float));

                                            totalNumberVertices += 3;
                                            numberIndicesGroup += 3;
                                        }
                                }
                            if (vnIndex >= 0)
                                {
                                    if (edgeCount < 3)
                                        {
                                            if (totalNumberNormals >= OM_MAX_TRIANGLE_ATTRIBUTES)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleNormals[3 * totalNumberNormals], &normals[3 * vnIndex], 3 * sizeof(float));

                                            totalNumberNormals++;
                                        }
                                    else
                                        {
                                            if (totalNumberNormals >= OM_MAX_TRIANGLE_ATTRIBUTES - 2)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleNormals[3 * (totalNumberNormals)],
                                                   &triangleNormals[3 * (totalNumberNormals - edgeCount)], 3 * sizeof(float));
                                            memcpy(&triangleNormals[3 * (totalNumberNormals + 1)],
                                                   &triangleNormals[3 * (totalNumberNormals - 1)], 3 * sizeof(float));
                                            memcpy(&triangleNormals[3 * (totalNumberNormals + 2)],
                                                   &normals[3 * vnIndex], 3 * sizeof(float));

                                            totalNumberNormals += 3;
                                        }
                                }
                            if (vtIndex >= 0)
                                {
                                    if (edgeCount < 3)
                                        {
                                            if (totalNumberTexCoords >= OM_MAX_TRIANGLE_ATTRIBUTES)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleTexCoords[2 * totalNumberTexCoords],
                                                   &texCoords[2 * vtIndex], 2 * sizeof(float));

                                            totalNumberTexCoords++;
                                        }
                                    else
                                        {
                                            if (totalNumberTexCoords >= OM_MAX_TRIANGLE_ATTRIBUTES - 2)
                                                {
                                                    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices,
                                                                     &triangleNormals, &triangleTexCoords);

                                                    fclose(f);

                                                    return GL_FALSE;
                                                }

                                            memcpy(&triangleTexCoords[2 * (totalNumberTexCoords)],
                                                   &triangleTexCoords[2 * (totalNumberTexCoords - edgeCount)], 2 * sizeof(float));
                                            memcpy(&triangleTexCoords[2 * (totalNumberTexCoords + 1)],
                                                   &triangleTexCoords[2 * (totalNumberTexCoords - 1)], 2 * sizeof(float));
                                            memcpy(&triangleTexCoords[2 * (totalNumberTexCoords + 2)],
                                                   &texCoords[2 * vtIndex], 2 * sizeof(float));

                                            totalNumberTexCoords += 3;
                                        }
                                }

                            edgeCount++;

                            token = strtok(0, " \n");
                        }
                }
        }

    fclose(f);

    if (wavefront && currentGroupList)
        {
            currentGroupList->group.numberIndices = numberIndicesGroup;
            numberIndicesGroup = 0;
        }

    result = OMCopyObjData(shape, totalNumberVertices, triangleVertices, totalNumberNormals,
                           triangleNormals, totalNumberTexCoords, triangleTexCoords);

    OMFreeTempMemory(&vertices, &normals, &texCoords, &triangleVertices, &triangleNormals, &triangleTexCoords);

    if (result)
        {
            OMCalculateTangentSpacef(shape);
        }

    return result;
}

void VertexGenerator::OMInitMaterial(OMmaterial* material)
{
    if (!material)
        {
            return;
        }

    material->name[0] = 0;

    material->emissive[0] = 0.0f;
    material->emissive[1] = 0.0f;
    material->emissive[2] = 0.0f;
    material->emissive[3] = 1.0f;

    material->ambient[0] = 0.0f;
    material->ambient[1] = 0.0f;
    material->ambient[2] = 0.0f;
    material->ambient[3] = 1.0f;

    material->diffuse[0] = 0.0f;
    material->diffuse[1] = 0.0f;
    material->diffuse[2] = 0.0f;
    material->diffuse[3] = 1.0f;

    material->specular[0] = 0.0f;
    material->specular[1] = 0.0f;
    material->specular[2] = 0.0f;
    material->specular[3] = 1.0f;

    material->shininess = 0.0f;

    material->transparency = 1.0f;

    material->reflection = false;

    material->refraction = false;

    material->indexOfRefraction = 1.0f;

    material->ambientTextureFilename[0] = 0;

    material->diffuseTextureFilename[0] = 0;

    material->specularTextureFilename[0] = 0;

    material->transparencyTextureFilename[0] = 0;

    material->bumpTextureFilename[0] = 0;

    material->ambientTextureName = 0;

    material->diffuseTextureName = 0;

    material->specularTextureName = 0;

    material->transparencyTextureName = 0;

    material->bumpTextureName = 0;
}

bool VertexGenerator::OMLoadMaterial(const char* filename, OMmaterialList** materialList)
{
    FILE* f;

    int i, k;

    char buffer[OM_BUFFERSIZE];
    char* checkBuffer;
    char name[OM_MAX_STRING];
    char identifier[7];

    OMmaterialList* currentMaterialList = 0;

    if (!filename || !materialList)
        {
            return false;
        }

    f = fopen(filename, "r");

    if (!f)
        {
            return false;
        }

    while (!feof(f))
        {
            if (fgets(buffer, OM_BUFFERSIZE, f) == 0)
                {
                    if (ferror(f))
                        {
                            fclose(f);

                            return false;
                        }
                }

            checkBuffer = buffer;

            k = 0;

            // Skip first spaces etc.
            while (*checkBuffer)
                {
                    if (*checkBuffer != ' ' && *checkBuffer != '\t')
                        {
                            break;
                        }

                    checkBuffer++;
                    k++;

                    if (k >= OM_BUFFERSIZE)
                        {
                            fclose(f);

                            return false;
                        }
                }

            i = 0;

            while (checkBuffer[i])
                {
                    if (checkBuffer[i] == ' ' || checkBuffer[i] == '\t')
                        {
                            break;
                        }

                    checkBuffer[i] = tolower(checkBuffer[i]);

                    i++;

                    if (i >= OM_BUFFERSIZE - k)
                        {
                            fclose(f);

                            return false;
                        }
                }

            if (strncmp(checkBuffer, "newmtl", 6) == 0)
                {
                    OMmaterialList* newMaterialList = 0;

                    sscanf(checkBuffer, "%s %s", identifier, name);

                    newMaterialList = (OMmaterialList*)malloc(sizeof(OMmaterialList));

                    if (!newMaterialList)
                        {
                            OMDestroyMaterial(materialList);

                            fclose(f);

                            return false;
                        }

                    memset(newMaterialList, 0, sizeof(OMmaterialList));

                    OMInitMaterial(&newMaterialList->material);

                    strcpy(newMaterialList->material.name, name);

                    if (*materialList == 0)
                        {
                            *materialList = newMaterialList;
                        }
                    else
                        {
                            currentMaterialList->next = newMaterialList;
                        }

                    currentMaterialList = newMaterialList;
                }
            else if (strncmp(checkBuffer, "ke", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f %f %f", identifier, &currentMaterialList->material.emissive[0],
                           &currentMaterialList->material.emissive[1], &currentMaterialList->material.emissive[2]);

                    currentMaterialList->material.emissive[3] = 1.0f;
                }
            else if (strncmp(checkBuffer, "ka", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f %f %f", identifier, &currentMaterialList->material.ambient[0],
                           &currentMaterialList->material.ambient[1], &currentMaterialList->material.ambient[2]);

                    currentMaterialList->material.ambient[3] = 1.0f;
                }
            else if (strncmp(checkBuffer, "kd", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f %f %f", identifier, &currentMaterialList->material.diffuse[0],
                           &currentMaterialList->material.diffuse[1], &currentMaterialList->material.diffuse[2]);

                    currentMaterialList->material.diffuse[3] = 1.0f;
                }
            else if (strncmp(checkBuffer, "ks", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f %f %f", identifier, &currentMaterialList->material.specular[0],
                           &currentMaterialList->material.specular[1], &currentMaterialList->material.specular[2]);

                    currentMaterialList->material.specular[3] = 1.0f;
                }
            else if (strncmp(checkBuffer, "ns", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f", identifier, &currentMaterialList->material.shininess);
                }
            else if (strncmp(checkBuffer, "d", 1) == 0 || strncmp(checkBuffer, "Tr", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f", identifier, &currentMaterialList->material.transparency);
                }
            else if (strncmp(checkBuffer, "ni", 2) == 0)
                {
                    sscanf(checkBuffer, "%s %f", identifier, &currentMaterialList->material.indexOfRefraction);
                }
            else if (strncmp(checkBuffer, "map_ka", 6) == 0)
                {
                    sscanf(checkBuffer, "%s %s", identifier, name);

                    strcpy(currentMaterialList->material.ambientTextureFilename, name);
                }
            else if (strncmp(checkBuffer, "map_kd", 6) == 0)
                {
                    sscanf(checkBuffer, "%s %s", identifier, name);

                    strcpy(currentMaterialList->material.diffuseTextureFilename, name);
                }
            else if (strncmp(checkBuffer, "map_ks", 6) == 0)
                {
                    sscanf(checkBuffer, "%s %s", identifier, name);

                    strcpy(currentMaterialList->material.specularTextureFilename, name);
                }
            else if (strncmp(checkBuffer, "map_d", 5) == 0)
                {
                    sscanf(checkBuffer, "%s %s", identifier, name);

                    strcpy(currentMaterialList->material.transparencyTextureFilename, name);
                }
            else if (strncmp(checkBuffer, "map_bump", 8) == 0 || strncmp(checkBuffer, "bump", 4) == 0)
                {
                    sscanf(checkBuffer, "%s %s", identifier, name);

                    strcpy(currentMaterialList->material.bumpTextureFilename, name);
                }
            else if (strncmp(checkBuffer, "illum", 5) == 0)
                {
                    int illum;

                    sscanf(checkBuffer, "%s %d", identifier, &illum);

                    // Only setting reflection and refraction depending on illumination model.
                    switch (illum)
                        {
                            case 3:
                            case 4:
                            case 5:
                            case 8:
                            case 9:
                                currentMaterialList->material.reflection = true;
                                break;
                            case 6:
                            case 7:
                                currentMaterialList->material.reflection = true;
                                currentMaterialList->material.refraction = true;
                                break;
                        }
                }
        }

    fclose(f);

    return true;
}

void VertexGenerator::OMDestroyMaterial(OMmaterialList** materialList)
{
    OMmaterialList* currentMaterialList = 0;
    OMmaterialList* nextMaterialList = 0;

    if (!materialList)
        {
            return;
        }

    currentMaterialList = *materialList;
    while (currentMaterialList != 0)
        {
            nextMaterialList = currentMaterialList->next;

            memset(&currentMaterialList->material, 0, sizeof(OMmaterial));

            free(currentMaterialList);

            currentMaterialList = nextMaterialList;
        }

    *materialList = 0;
}

void VertexGenerator::destroyObjectModel(OBJModel* shape)
{
    if (!shape)
        {
            return;
        }

    if (shape->vertices)
        {
            free(shape->vertices);

            shape->vertices = 0;
        }

    if (shape->normals)
        {
            free(shape->normals);

            shape->normals = 0;
        }

    if (shape->tangents)
        {
            free(shape->tangents);

            shape->tangents = 0;
        }

    if (shape->bitangents)
        {
            free(shape->bitangents);

            shape->bitangents = 0;
        }

    if (shape->texCoords)
        {
            free(shape->texCoords);

            shape->texCoords = 0;
        }

    if (shape->allAttributes)
        {
            free(shape->allAttributes);

            shape->allAttributes = 0;
        }

    if (shape->indices)
        {
            free(shape->indices);

            shape->indices = 0;
        }

    shape->numberVertices = 0;
    shape->numberIndices = 0;
    shape->mode = 0;
}

GLboolean VertexGenerator::OMCopyObjData(OBJModel* shape, unsigned short totalNumberVertices,
        float* triangleVertices, unsigned short totalNumberNormals, float* triangleNormals,
        unsigned short totalNumberTexCoords, float* triangleTexCoords)
{
    unsigned int indicesCounter = 0;

    if (!shape || !triangleVertices || !triangleNormals || !triangleTexCoords)
        {
            return false;
        }

    shape->numberVertices = totalNumberVertices;

    if (totalNumberVertices > 0)
        {
            shape->vertices = (float*)malloc(totalNumberVertices * 4 * sizeof(float));

            if (shape->vertices == 0)
                {
                    destroyObjectModel(shape);

                    return false;
                }

            memcpy(shape->vertices, triangleVertices, totalNumberVertices * 4 * sizeof(float));
        }
    if (totalNumberNormals > 0)
        {
            shape->normals = (float*)malloc(totalNumberNormals * 3 * sizeof(float));

            if (shape->normals == 0)
                {
                    destroyObjectModel(shape);

                    return false;
                }

            memcpy(shape->normals, triangleNormals, totalNumberNormals * 3 * sizeof(float));
        }
    if (totalNumberTexCoords > 0)
        {
            shape->texCoords = (float*)malloc(totalNumberTexCoords * 2 * sizeof(float));

            if (shape->texCoords == 0)
                {
                    destroyObjectModel(shape);

                    return false;
                }

            memcpy(shape->texCoords, triangleTexCoords, totalNumberTexCoords * 2 * sizeof(float));
        }

    // Just create the indices from the list of vertices.

    shape->numberIndices = totalNumberVertices;

    if (totalNumberVertices > 0)
        {
            shape->indices = (unsigned short*)malloc(totalNumberVertices * sizeof(unsigned short));

            if (shape->indices == 0)
                {
                    destroyObjectModel(shape);

                    return false;
                }

            for (indicesCounter = 0; indicesCounter < totalNumberVertices; indicesCounter++)
                {
                    shape->indices[indicesCounter] = indicesCounter;
                }
        }

    shape->mode = GL_TRIANGLES;

    return true;
}

bool VertexGenerator::OMMallocTempMemory(float** vertices, float** normals, float** texCoords,
        float** triangleVertices, float** triangleNormals, float** triangleTexCoords)
{
    if (!vertices || !normals || !texCoords || !triangleVertices || !triangleNormals || !triangleTexCoords)
        {
            LOG_ERROR("ObjModel: MallocTempMemory failed input parameters!\n");
            return false;
        }

    *vertices = (float*)malloc(4 * OM_MAX_ATTRIBUTES * sizeof(float));
    if (!*vertices)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    *normals = (float*)malloc(3 * OM_MAX_ATTRIBUTES * sizeof(float));
    if (!*normals)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    *texCoords = (float*)malloc(2 * OM_MAX_ATTRIBUTES * sizeof(float));
    if (!*texCoords)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    *triangleVertices = (float*)malloc(4 * OM_MAX_TRIANGLE_ATTRIBUTES * sizeof(float));
    if (!*triangleVertices)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    *triangleNormals = (float*)malloc(3 * OM_MAX_TRIANGLE_ATTRIBUTES * sizeof(float));
    if (!*triangleNormals)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    *triangleTexCoords = (float*)malloc(2 * OM_MAX_TRIANGLE_ATTRIBUTES * sizeof(float));
    if (!*triangleTexCoords)
        {
            LOG_ERROR("ObjModel: malloc failed! +%d\n", __LINE__);
            return false;
        }

    return true;
}

void VertexGenerator::OMFreeTempMemory(float** vertices, float** normals, float** texCoords,
                                       float** triangleVertices, float** triangleNormals, float** triangleTexCoords)
{
    if (vertices && *vertices)
        {
            free(*vertices);

            *vertices = 0;
        }

    if (normals && *normals)
        {
            free(*normals);

            *normals = 0;
        }

    if (texCoords && *texCoords)
        {
            free(*texCoords);

            *texCoords = 0;
        }

    if (triangleVertices && *triangleVertices)
        {
            free(*triangleVertices);

            *triangleVertices = 0;
        }

    if (triangleNormals && *triangleNormals)
        {
            free(*triangleNormals);

            *triangleNormals = 0;
        }

    if (triangleTexCoords && *triangleTexCoords)
        {
            free(*triangleTexCoords);

            *triangleTexCoords = 0;
        }
}

}
