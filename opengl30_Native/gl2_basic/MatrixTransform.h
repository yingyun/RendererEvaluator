/*
* FixMe; Add comments
*
*
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
};


}

#endif
