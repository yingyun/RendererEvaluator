#ifndef  VERTEXGENERATOR_H
#define VERTEXGENERATOR_H

#include <stdio.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <string>

#include "Pattern/Singleton.h"
#include "Logging.h"

using std::string;

namespace RenderEvaluator
{

class VertexGenerator : public Singleton<VertexGenerator>
{
public:
    VertexGenerator() {}
    bool genObjectModel(string objName);

    /* ------------------------------Mesh2D------------------------------ */
    class Mesh2D {
    public:
        enum Primitive {
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
        class VertexArray {
            friend class Mesh2D;
            float* mData;
            size_t mStride;
            VertexArray(float* data, size_t stride) : mData(data), mStride(stride) { }
        public:
            TYPE& operator[](size_t index) {
                return *reinterpret_cast<TYPE*>(&mData[index*mStride]);   //Magic here ....
            }
            TYPE const& operator[](size_t index) const {
                return *reinterpret_cast<TYPE const*>(&mData[index*mStride]);
            }
        };

        Mesh2D(): mPrimitive(TRIANGLE_FAN), mVertices(0), mVertexCount(4), 
            mVertexSize(2), mTexCoordsSize(2) 
            {
                //do something in the future
            }
        
        Mesh2D(Mesh2D& mesh)
            {
                mPrimitive = mesh.getPrimitive();
                mVertexCount = mesh.getVertexCount();
                mVertexSize = mesh.getVertexSize();
                mTexCoordsSize = mesh.getTexCoordsSize();
                mStride = mesh.getStride();
                mVertices = mesh.getPositions();
                if(mVertices == 0)
                    {
                        LOG_ERROR("Wrong Mesh2D init!\n");
                        exit(-1);
                    }
            }

        Mesh2D(Primitive primitive, size_t vertexCount, 
            size_t vertexSize, size_t texCoordsSize): 
            mPrimitive(primitive), 
            mVertexCount(vertexCount),
            mVertexSize(vertexSize),
            mTexCoordsSize(texCoordsSize)
            {
                mVertices = new float[(mVertexSize + mTexCoordsSize) * mVertexCount];
                mStride = mVertexSize + mTexCoordsSize;
            }
            
        ~Mesh2D()
            {
                //delete [] mVertices;,,, Keep this until program was terminated
            }

        template <typename TYPE>
        VertexArray<TYPE> getPositionArray() { 
            return VertexArray<TYPE>(getPositions(), mStride); }

        template <typename TYPE>
        VertexArray<TYPE> getTexCoordArray() { 
            return VertexArray<TYPE>(getTexCoords(), mStride); }

        Primitive getPrimitive() { return mPrimitive; }
        float* getPositions() { return mVertices; }
        float* getTexCoords() { return mVertices + mVertexSize; }
        size_t getVertexCount() { return mVertexCount; }
        size_t getVertexSize() { return mVertexSize; }
        size_t getTexCoordsSize() { return mTexCoordsSize; }
        size_t getByteStride() { return mStride*sizeof(float); }
        size_t getStride() { return mStride; }
        void dumpInfo()
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
    class VEC2 {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union {
            struct { 
                TYPE x; TYPE y; 
                };
            struct { 
                TYPE s; TYPE t; 
                };
        };

        enum { SIZE = 2 };
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference       operator [] (size_t i)       { return (&x)[i]; }

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
    class VEC3 {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union {
            struct { 
                TYPE x; TYPE y; TYPE z;
                };
            struct { 
                TYPE r; TYPE g; TYPE b; 
                };
        };

        enum { SIZE = 3 };
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference       operator [] (size_t i)       { return (&x)[i]; }

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
    class VEC4 {
    public:
        typedef TYPE& reference;
        typedef TYPE const& const_reference;
        typedef size_t size_type;

        union {
            struct { 
                TYPE x; TYPE y; TYPE z; TYPE w;
                };
            struct { 
                TYPE r; TYPE g; TYPE b; TYPE a; 
                };
        };

        enum { SIZE = 4 };
        inline static size_type size() { return SIZE; }
        inline const_reference operator [] (size_t i) const { return (&x)[i]; }
        inline reference       operator [] (size_t i)       { return (&x)[i]; }

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

};

}

#endif
