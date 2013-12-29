/*
*2013 Cui.Yingyun
* Vairous affine transform matrix
*
*/


#ifndef  MATRIXTRANSFORM_H
#define MATRIXTRANSFORM_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


namespace android
{

typedef struct
{
    GLfloat m[4][4];
} Matrix44;

typedef struct
{
    GLfloat v[4];
} Vector4;

class MatrixTransform
{
    //FixMe; Implement RTS firstly

private:

public:

    //Basic Matrix operation
    static void matrixIndentity(Matrix44 *result);
    static void matrixMultiply(Matrix44 *result, Matrix44 *srcA, Matrix44 *srcB);
    static void matrixRotate(Matrix44 * result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    static void matrixTranslate(Matrix44 * result, GLfloat x, GLfloat y, GLfloat z);
    static void matrixScale(Matrix44 * result, GLfloat sx, GLfloat sy, GLfloat sz);
    static void matrixOrthoProjection(Matrix44* result, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
    static void matrixDump(const Matrix44 * M, const char * tag);
    static void vectorDump(const Vector4 * vDumped);

};


}

#endif
