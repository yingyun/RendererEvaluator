/*
*polygon generater.
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

//TODO: Reimplement below c-style string processing by C++ String style.
bool VertexGenerator::loadObjModel(string objName, float** o_vertices, float** o_uvs,
                                   float** o_normals, unsigned int* o_vertexCount)
{
    vector<VEC3_F> vertexDatas;
    vector<VEC2_F> uvDatas;
    vector<VEC3_F> normalDatas;
    vector<unsigned int> vertexIndices;
    vector<unsigned int> uvIndices;
    vector<unsigned int> normalIndices;

    bool hasVertex = false;
    bool hasUV = false;
    bool hasNormal = false;

    string  filleName = string(OBJ_MODEL_PATH) + string(objName);
    const char* filePath = filleName .data();

    LOG_INFO("VertexGenerator::loadObjModel file %s...\n", filePath);

    FILE * file = fopen(filePath, "r");
    if( file == NULL )
        {
            LOG_ERROR("%s Open %s failed!!! Right path???\n", __func__, filePath);
            return false;
        }

    char lineBuffer[ONE_LINE_SIZE];
    while(true)
        {
            if(fgets(lineBuffer, ONE_LINE_SIZE, file) == 0)
                {
                    if(ferror(file))
                        {
                            fclose(file);
                            LOG_ERROR("%s Get oneline buffer failed!!!\n", __func__);
                            return false;
                        }
                    if(feof(file) != 0)
                        {
                            fclose(file);
                            LOG_INFO("ObjectModel file scaner met end of file!\n");
                            break;
                        }
                }

            /*
            *Scanning the whole line one by one to retrive attributes data.
            *Watch out the order of judging the identifier
            */
            char identifier[5];
            if (strncmp( lineBuffer, "vt", 2) == 0)
                {
                    VEC2_F uv;
                    sscanf(lineBuffer, "%s %f %f\n", identifier, &uv[0], &uv[1]);
                    /*
                    *NOTE: Invert V coordinate since we will only use DDS texture, which are inverted.
                    *Remove if you want to use TGA or BMP loaders.
                    */
                    uv[1] = -uv[1];
                    uvDatas.push_back(uv);

                    LOG_DEBUG("%s-> %f, %f\n",identifier, uv[0], uv[1]);
                }
            else if (strncmp( lineBuffer, "vn", 2) == 0)
                {
                    VEC3_F normal;
                    sscanf(lineBuffer, "%s %f %f %f\n",identifier, &normal[0], &normal[1], &normal[2]);
                    normalDatas.push_back(normal);

                    LOG_DEBUG("%s-> %f, %f, %f\n", identifier, normal[0], normal[1], normal[2]);
                }
            else if (strncmp( lineBuffer, "v", 1 ) == 0)
                {
                    VEC3_F vertex;
                    sscanf(lineBuffer, "%s %f %f %f\n",identifier, &vertex[0], &vertex[1], &vertex[2]);
                    vertexDatas.push_back(vertex);

                    LOG_DEBUG("%s-> %f, %f, %f\n", identifier, vertex[0], vertex[1], vertex[2]);
                }
            else if (strncmp( lineBuffer, "f", 1) == 0)
                {
                    enum FACE_TYPE
                    {
                        VERTEX_UV_NORMAL,
                        VERTEX_UV,
                        VERTEX_NORMAL,
                        VERTEX,
                    };
                    enum FACE_TYPE faceFormat;
                    //face format :::  f 5//3 6//3 2//3
                    char septs[] = " \n";
                    char* token;
                    token = strtok(lineBuffer, septs);
                    token = strtok(0, septs);

                    if(strstr(token, "//") != 0)
                        {
                            faceFormat = VERTEX_NORMAL;
                        }
                    else if(strstr(token, "/") == 0)
                        {
                            faceFormat = VERTEX;
                        }
                    else if(strstr(token, "/") != 0)
                        {
                            char* first_slash = strstr(token, "/");
                            first_slash++;
                            if(!first_slash)
                                {
                                    LOG_ERROR("%s Can't be happned!!!\n", __func__);
                                    return false;
                                }

                            if(strstr(first_slash, "/") == 0)
                                {
                                    faceFormat = VERTEX_UV;
                                }
                            else
                                {
                                    faceFormat = VERTEX_UV_NORMAL;
                                }
                        }

                    unsigned int vertexIndex;
                    unsigned int uvIndex;
                    unsigned int normalIndex;

                    while(token != 0)
                        {
                            switch(faceFormat)
                                {
                                    case VERTEX_UV_NORMAL:
                                    {
                                        hasVertex = true;
                                        hasUV = true;
                                        hasNormal = true;
                                        int matches = sscanf(token, "%d/%d/%d", &vertexIndex, &uvIndex, &normalIndex);
                                        vertexIndices.push_back(vertexIndex);
                                        uvIndices.push_back(uvIndex);
                                        normalIndices.push_back(normalIndex);

                                        LOG_DEBUG("Face Parsing-> V_U_N: %d %d %d\n", vertexIndex, uvIndex, normalIndex);
                                    }
                                    break;
                                    case VERTEX_UV:
                                    {
                                        hasVertex = true;
                                        hasUV = true;
                                        int matches = sscanf(token, "%d/%d", &vertexIndex, &uvIndex);
                                        vertexIndices.push_back(vertexIndex);
                                        uvIndices.push_back(uvIndex);

                                        LOG_DEBUG("Face Parsing-> V_U: %d %d\n", vertexIndex, uvIndex);
                                    }
                                    break;
                                    case VERTEX_NORMAL:
                                    {
                                        hasVertex = true;
                                        hasNormal = true;
                                        int matches = sscanf(token, "%d//%d", &vertexIndex, &normalIndex);
                                        vertexIndices.push_back(vertexIndex);
                                        normalIndices.push_back(normalIndex);

                                        LOG_DEBUG("Face Parsing-> V_N: %d %d\n", vertexIndex, normalIndex);
                                    }
                                    break;
                                    case VERTEX:
                                    {
                                        hasVertex = true;
                                        int matches = sscanf(token, "%d", &vertexIndex);
                                        vertexIndices.push_back(vertexIndex);

                                        LOG_DEBUG("Face Parsing-> V: %d\n", vertexIndex);
                                    }
                                    break;
                                }
                            //Swipe to next token
                            token = strtok(0, " \n");
                        }
                }
        }

    //Create data pool which exported to the caller,
    //NOTE that the caller also has guarantee to release it by calling unloadObjModel
    unsigned int numVertex = vertexIndices.size();
    *o_vertexCount = numVertex;
    LOG_INFO("loadObjModel: %d number of vertex we have imported!\n", *o_vertexCount);

    if(hasVertex && (o_vertices != NULL))
        {
            *o_vertices = new float[sizeof(float) * VERTEX_C * numVertex];
            if(*o_vertices == NULL)
                {
                    LOG_ERROR("loadObjModel: Allocate vertics pool Faield!\n");
                    return false;
                }
            LOG_INFO("\t->Allocate vertices pool!\n");
        }
    if(hasUV && (o_uvs != NULL))
        {
            *o_uvs = new float[sizeof(float) * UV_C * numVertex];
            if(*o_uvs == NULL)
                {
                    LOG_ERROR("loadObjModel: Allocate UVs pool Faield!\n");
                    return false;
                }
            LOG_INFO("\t->Allocate UVs pool!\n");
        }
    if(hasNormal && (o_normals != NULL))
        {
            *o_normals = new float[sizeof(float) * NORMAL_C * numVertex];
            if(*o_normals == NULL)
                {
                    LOG_ERROR("loadObjModel: Allocate Normals pool Faield!\n");
                    return false;
                }
            LOG_INFO("\t->Allocate normals pool!\n");
        }

    for(unsigned int i = 0; i < vertexIndices.size(); i++)
        {
            if(hasVertex && (o_vertices != NULL))
                {
                    unsigned int vertexIndex = vertexIndices[i];
                    VEC3_F vertex = vertexDatas[vertexIndex - 1];

                    float* vertex_base = *o_vertices;
                    vertex_base[i * VERTEX_C + VER_X] = vertex[VER_X];
                    vertex_base[i * VERTEX_C + VER_Y] = vertex[VER_Y];
                    vertex_base[i * VERTEX_C + VER_Z] = vertex[VER_Z];
                }
            if(hasUV && (o_uvs != NULL))
                {
                    unsigned int uvIndex = uvIndices[i];
                    VEC2_F uv = uvDatas[uvIndex - 1];

                    float* uv_base = *o_uvs;
                    uv_base[i * UV_C + TEX_U] = uv[TEX_U];
                    uv_base[i * UV_C + TEX_Y] = uv[TEX_Y];
                }
            if(hasNormal && (o_normals != NULL))
                {
                    unsigned int normalIndex = normalIndices[i];
                    VEC3_F normal = normalDatas[normalIndex - 1];

                    float* normal_base = *o_normals;
                    normal_base[i * NORMAL_C + NOR_X] = normal[NOR_X];
                    normal_base[i * NORMAL_C + NOR_Y] = normal[NOR_Y];
                    normal_base[i * NORMAL_C + NOR_Z] = normal[NOR_Z];
                }
        }
    return true;
}

void VertexGenerator::unloadObjModel(float** vertices_addr, float** uvs_addr, float** normals_addr)
{
    LOG_INFO("VertexGenerator::unloadObjModel...\n");
    if(vertices_addr != NULL && *vertices_addr != NULL)
        {
            delete [] *vertices_addr;
            *vertices_addr = NULL;
            LOG_INFO("\t->Release vertices pool!\n");
        }

    if(uvs_addr != NULL && *uvs_addr != NULL)
        {
            delete [] *uvs_addr;
            *uvs_addr = NULL;
            LOG_INFO("\t->Release UVs pool!\n");
        }

    if(normals_addr != NULL && *normals_addr != NULL)
        {
            delete [] *normals_addr;
            *normals_addr = NULL;
            LOG_INFO("\t->Release normals pool!\n");
        }
}






}
