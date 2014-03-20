/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/

#ifndef  VERTEXGENERATOR_H
#define VERTEXGENERATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>

#if USE_OPENGL_ES_VER == 30
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif USE_OPENGL_ES_VER == 20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "Pattern/Singleton.h"
#include "Logging.h"

using std::string;

namespace RenderEvaluator
{

#define OM_BUFFERSIZE 1024
#define OM_MAX_STRING  256
#define OM_MAX_VERTICES 1048576
#define OM_VERTICES_DIVISOR 4
#define OM_MAX_ATTRIBUTES (OM_MAX_VERTICES/OM_VERTICES_DIVISOR)
#define OM_MAX_TRIANGLE_ATTRIBUTES OM_MAX_VERTICES
#define OBJ_MODEL_PATH "/data/RenderEvaluator/"

class VertexGenerator : public Singleton<VertexGenerator>
{
public:
    VertexGenerator() {}

    /* ------------------------------Mesh2D------------------------------ */
    class Mesh2D
    {
    public:
        enum Primitive
        {
            POINTS          = GL_POINTS,
            LINES           = GL_LINES,
            LINE_LOOP       = GL_LINE_LOOP,
            LINE_STRIP      = GL_LINE_STRIP,
            TRIANGLES       = GL_TRIANGLES,
            TRIANGLE_STRIP  = GL_TRIANGLE_STRIP,
            TRIANGLE_FAN    = GL_TRIANGLE_FAN
        };

        /* ------------------------------Vertex allocator------------------------------ */
        template <typename TYPE>
        class VertexArray
        {
            friend class Mesh2D;
            float* mData;
            size_t mStride;
            VertexArray(float* data, size_t stride) : mData(data), mStride(stride) { }
        public:
            TYPE& operator[](size_t index)
            {
                return *reinterpret_cast<TYPE*>(&mData[index*mStride]);   //Magic here ....
            }
            TYPE const& operator[](size_t index) const
            {
                return *reinterpret_cast<TYPE const*>(&mData[index*mStride]);
            }
        };


        Mesh2D(): mPrimitive(TRIANGLE_FAN), mVertices(0), mVertexCount(4),
            mVertexSize(2), mTexCoordsSize(2)
        {
            //do something in the future
        }
        Mesh2D(Primitive primitive, size_t vertexCount, size_t vertexSize, size_t texCoordsSize);
        ~Mesh2D();

        Mesh2D& operator = (Mesh2D& mesh);

        template <typename TYPE>
        inline VertexArray<TYPE> getPositionArray()
        {
            return VertexArray<TYPE>(getPositions(), mStride);
        }

        template <typename TYPE>
        inline VertexArray<TYPE> getTexCoordArray()
        {
            return VertexArray<TYPE>(getTexCoords(), mStride);
        }

        inline Primitive getPrimitive()
        {
            return mPrimitive;
        }
        inline float* getPositions()
        {
            return mVertices;
        }
        inline float* getTexCoords()
        {
            return mVertices + mVertexSize;
        }
        inline size_t getVertexCount()
        {
            return mVertexCount;
        }
        inline size_t getVertexSize()
        {
            return mVertexSize;
        }
        inline size_t getTexCoordsSize()
        {
            return mTexCoordsSize;
        }
        inline size_t getByteStride()
        {
            return mStride*sizeof(float);
        }
        inline size_t getStride()
        {
            return mStride;
        }
        void dumpInfo();

    private:
        Primitive mPrimitive;
        float* mVertices;
        size_t mVertexCount;
        size_t mVertexSize;
        size_t mTexCoordsSize;
        size_t mStride;
    };


    /* ------------------------------VEC2 implementation------------------------------ */
    template <typename TYPE>
    class VEC2
    {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union
        {
            struct
            {
                TYPE x;
                TYPE y;
            };
            struct
            {
                TYPE s;
                TYPE t;
            };
        };

        enum { SIZE = 2 };
        inline static size_type size()
        {
            return SIZE;
        }
        inline const_reference operator [] (size_t i) const
        {
            return (&x)[i];
        }
        inline reference       operator [] (size_t i)
        {
            return (&x)[i];
        }

        VEC2() : x(0), y(0) { }

        template<typename A>
        VEC2(A v) : x(v), y(v) { }

        template<typename A> //How to distiguish wiht above constructor??
        explicit VEC2(const VEC2<A>& v) : x(v.x), y(v.y) { }

        template<typename A, typename B>
        VEC2(A x, B y) : x(x), y(y) { }
    };

    typedef VEC2<float> VEC2_F;
    typedef VEC2<int> VEC2_I;

    /* ------------------------------VEC3 implementation------------------------------ */
    template <typename TYPE>
    class VEC3
    {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union
        {
            struct
            {
                TYPE x;
                TYPE y;
                TYPE z;
            };
            struct
            {
                TYPE r;
                TYPE g;
                TYPE b;
            };
        };

        enum { SIZE = 3 };
        inline static size_type size()
        {
            return SIZE;
        }
        inline const_reference operator [] (size_t i) const
        {
            return (&x)[i];
        }
        inline reference       operator [] (size_t i)
        {
            return (&x)[i];
        }

        VEC3() : x(0), y(0), z(0) { }

        template<typename A>
        VEC3(A v) : x(v), y(v), z(v) { }

        template<typename A>  //How to distiguish wiht above constructor??
        explicit VEC3(const VEC3<A>& v) : x(v.x), y(v.y), z(v.z) { }

        template<typename A, typename B, typename C>
        VEC3(A x, B y, C z) : x(x), y(y), z(z) { }
    };

    typedef VEC3<float> VEC3_F;
    typedef VEC3<int> VEC3_I;

    /* ------------------------------VEC4 implementation------------------------------ */
    template <typename TYPE>
    class VEC4
    {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union
        {
            struct
            {
                TYPE x;
                TYPE y;
                TYPE z;
                TYPE w;
            };
            struct
            {
                TYPE r;
                TYPE g;
                TYPE b;
                TYPE a;
            };
        };

        enum { SIZE = 4 };
        inline static size_type size()
        {
            return SIZE;
        }
        inline const_reference operator [] (size_t i) const
        {
            return (&x)[i];
        }
        inline reference       operator [] (size_t i)
        {
            return (&x)[i];
        }

        VEC4() : x(0), y(0), z(0), w(0) { }

        template<typename A>
        VEC4(A v) : x(v), y(v), z(v), w(v) { }

        template<typename A>  //How to distiguish wiht above constructor??
        explicit VEC4(const VEC4<A>& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

        template<typename A, typename B, typename C, typename D>
        VEC4(A x, B y, C z, D w) : x(x), y(y), z(z), w(w) { }
    };

    typedef VEC4<float> VEC4_F;
    typedef VEC4<int> VEC4_I;

    /* ------------------------------Object model holder and export------------------------------ */
    typedef struct objectmodel
    {
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
    } OBJModel;

    bool genObjectModel(string objName, OBJModel* shape);
    void destroyObjectModel(OBJModel* shape);
    bool OMVector3Normalizef(float vector[3]);
    void OMVector3Crossf(float result[3], const float vector0[3], const float vector1[3]);

private:
    /*------------------------------Structure for holding material data.------------------------------*/
    typedef struct _OMmaterial
    {
        //Name of the material
        char name[OM_MAX_STRING];
        //Emissive color
        float emissive[4];
        //Ambient color
        float ambient[4];
        //Diffuse color
        float diffuse[4];
        //Specular color
        float specular[4];
        //Shininess
        float shininess;
        //Transparency, which is the alpha value
        float transparency;
        //Reflection
        bool reflection;
        //Refraction
        bool refraction;
        //Index of refraction
        float indexOfRefraction;
        //Ambient color texture filename
        char ambientTextureFilename[OM_MAX_STRING];
        //Diffuse color texture filename
        char diffuseTextureFilename[OM_MAX_STRING];
        //Specular color texture filename
        char specularTextureFilename[OM_MAX_STRING];
        //Transparency texture filename
        char transparencyTextureFilename[OM_MAX_STRING];
        //Bump texture filename
        char bumpTextureFilename[OM_MAX_STRING];
        //Can be used to store the ambient texture name
        unsigned int ambientTextureName;
        //Can be used to store the diffuse texture name
        unsigned int diffuseTextureName;
        //Can be used to store the specular texture name
        unsigned int specularTextureName;
        //Can be used to store the transparency texture name
        unsigned int transparencyTextureName;
        //Can be used to store the bump texture name
        unsigned int bumpTextureName;
    } OMmaterial;

    /* ------------------------------Structure for holding material data list ------------------------------*/
    typedef struct _OMmaterialList
    {
        //The material data.
        OMmaterial material;
        //The pointer to the next element.
        struct _OMmaterialList* next;
    } OMmaterialList;

    /*------------------------------Group of geometry.------------------------------*/
    typedef struct _OMgroup
    {
        //Name of the group
        char name[OM_MAX_STRING];
        //Name of the material
        char materialName[OM_MAX_STRING];
        //Pointer to the material.
        OMmaterial* material;
        //Indices
        unsigned int* indices;
        //Indices VBO
        unsigned int indicesVBO;
        //VAO of this group.
        unsigned int vao;
        //Number of indices
        unsigned int numberIndices;
        //Triangle render mode - could be either: GL_TRIANGLES   GL_TRIANGLE_STRIP
        unsigned int mode;
    } OMgroup;

    /*------------------------------Structure for holding the group data list.------------------------------*/
    typedef struct _OMgroupList
    {
        //The group data
        OMgroup group;
        //The pointer to the next group element
        struct _OMgroupList* next;
    } OMgroupList;

    /* ------------------------------Structure for a complete wavefront object file.------------------------------*/
    typedef struct _OMwavefront
    {
        //Vertices in homogeneous coordinates.
        float* vertices;
        //Vertices VBO.
        unsigned int verticesVBO;
        //Normals
        float* normals;
        //Normals VBO.
        unsigned int normalsVBO;
        //Tangents
        float* tangents;
        //Tangents VBO.
        unsigned int tangentsVBO;
        //Bitangents
        float* bitangents;
        //Bitangents VBO.
        unsigned int bitangentsVBO;
        //Texture coordinates
        float* texCoords;
        //Texture corrdinates VBO.
        unsigned int texCoordsVBO;
        //Number of vertices.
        unsigned int numberVertices;
        //Pointer to the first element of the groups.
        OMgroupList* groups;
        //Pointer to the first element of the materials.
        OMmaterialList* materials;
    } OMwavefront;

    /* ------------------------------ObjectModel Export method, Private------------------------------ */
    bool OMParseObjFile(string filePath, OBJModel* shape, OMwavefront* wavefront);
    float OMVector3Lengthf(const float vector[3]);
    void OMPoint4SubtractPoint4f(float result[3], const float point0[4], const float point1[4]);
    void OMDestroyMaterial(OMmaterialList** materialList);

    void OMInitMaterial(OMmaterial* material);
    bool OMCalculateTangentSpacef(OBJModel* shape);

    GLboolean OMCopyObjData(OBJModel* shape, unsigned short totalNumberVertices,
                            float* triangleVertices, unsigned short totalNumberNormals, float* triangleNormals,
                            unsigned short totalNumberTexCoords, float* triangleTexCoords);

    bool OMLoadMaterial(const char* filename, OMmaterialList** materialList);

    void OMFreeTempMemory(float** vertices, float** normals, float** texCoords,
                          float** triangleVertices, float** triangleNormals, float** triangleTexCoords);

    bool OMMallocTempMemory(float** vertices, float** normals, float** texCoords,
                            float** triangleVertices, float** triangleNormals, float** triangleTexCoords);
};

}

#endif
