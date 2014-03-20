/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef  MATRIXTRANSFORM_H
#define MATRIXTRANSFORM_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <EGL/egl.h>

#if USE_OPENGL_ES_VER == 30
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif USE_OPENGL_ES_VER == 20
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#include "Logging.h"
#include "Pattern/Singleton.h"
#include "VertexGenerator.h"

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

    void matrixIndentity(Matrix44* result);
    void matrixMultiply(Matrix44* result, Matrix44* srcA, Matrix44* srcB);

    /*---------------------Model matrix---------------------*/
    void matrixRotate(Matrix44* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void matrixScale(Matrix44* result, GLfloat sx, GLfloat sy, GLfloat sz);
    void matrixTranslate(Matrix44 * result, GLfloat x, GLfloat y, GLfloat z);

    /*---------------------View matrix---------------------*/
    void matrixLookAt(Matrix44* result,
                      const float eyeX, const float eyeY, const float eyeZ,
                      const float centerX, const float centerY, const float centerZ,
                      const float upX, const float upY, const float upZ);

    /*---------------------Projection matrix---------------------*/
    void matrixOrthoProjection(Matrix44* result, GLfloat left, GLfloat right, GLfloat bottom,
                               GLfloat top, GLfloat near, GLfloat far);

    void fullScreenOrthoProj(Matrix44* result, GLfloat width, GLfloat height);

    bool matrixPerspectiveProjection(Matrix44* result,
                                     const float fovy, const float aspect, const float zNear, const float zFar);

    /*---------------------Debug---------------------*/
    void matrixDump(const Matrix44* M, const char * tag);
    void vectorDump(const Vector4* vDumped);

private:
    bool matrixFrustumf(Matrix44* result,
                        const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal);

};

}
#endif
