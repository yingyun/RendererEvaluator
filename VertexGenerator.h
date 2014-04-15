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
#include <vector>

#if USE_OPENGL_ES_VER == 30
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif USE_OPENGL_ES_VER == 20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "Resource/ResourceProperty.h"
#include "Pattern/Singleton.h"
#include "Logging.h"

using std::string;
using std::vector;

namespace RenderEvaluator
{

#define ONE_LINE_SIZE 512
#define VERTEX_C 3
#define UV_C 2
#define NORMAL_C 3

#define VER_X 0
#define VER_Y 1
#define VER_Z 2

#define TEX_U 0
#define TEX_Y 1

#define NOR_X 0
#define NOR_Y 1
#define NOR_Z 2

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
        inline VertexArray<TYPE> getPositionArray() { return VertexArray<TYPE>(getPositions(), mStride); }
        template <typename TYPE>
        inline VertexArray<TYPE> getTexCoordArray() { return VertexArray<TYPE>(getTexCoords(), mStride); }
        inline Primitive getPrimitive() { return mPrimitive; }
        inline float* getPositions() { return mVertices; }
        inline float* getTexCoords() { return mVertices + mVertexSize; }
        inline size_t getVertexCount() { return mVertexCount; }
        inline size_t getVertexSize() { return mVertexSize; }
        inline size_t getTexCoordsSize() { return mTexCoordsSize; }
        inline size_t getByteStride() { return mStride*sizeof(float); }
        inline size_t getStride() { return mStride; }
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
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference operator [] (size_t i) { return (&x)[i]; }

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
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference operator [] (size_t i) { return (&x)[i]; }

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
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference operator [] (size_t i) { return (&x)[i]; }

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

    /*------------------------------Public API------------------------------*/
    bool loadObjModel(float** o_vertices, float** o_uvs,
                      float** o_normals, unsigned int* o_vertexCount, string objName);
    void unloadObjModel(float** vertices_addr, float** uvs_addr, float** normals_addr);
};

}

#endif
