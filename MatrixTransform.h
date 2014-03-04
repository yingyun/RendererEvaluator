#ifndef  MATRIXTRANSFORM_H
#define MATRIXTRANSFORM_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <EGL/egl.h>

#ifdef USE_OPENGL_ES_20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif 

#ifdef USE_OPENGL_ES_30
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

#include "Logging.h"
#include "Pattern/Singleton.h"

namespace RenderEvaluator
{
typedef struct
{
    GLfloat m[4][4];
} Matrix44;

typedef struct
{
    GLfloat v[4];
} Vector4;

class MatrixTransform : public Singleton<MatrixTransform>
{
public:
    MatrixTransform() {}
    void matrixIndentity(Matrix44 *result);
    void matrixMultiply(Matrix44 *result, Matrix44 *srcA, Matrix44 *srcB);
    void matrixRotate(Matrix44 * result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void matrixTranslate(Matrix44 * result, GLfloat x, GLfloat y, GLfloat z);
    void matrixScale(Matrix44 * result, GLfloat sx, GLfloat sy, GLfloat sz);
    void matrixOrthoProjection(Matrix44* result, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far);
    void matrixDump(const Matrix44 * M, const char * tag);
    void vectorDump(const Vector4 * vDumped);
    void androidStyleProjection(Matrix44 * result, GLfloat width, GLfloat height);
};
}
#endif
