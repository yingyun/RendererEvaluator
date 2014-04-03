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
#include "VectorTransform.h"

namespace RenderEvaluator
{

typedef struct
{
    GLfloat m[4][4];
} Matrix44;

typedef struct
{
    GLfloat m[3][3];
} Matrix33;

class MatrixTransform : public Singleton<MatrixTransform>
{
public:
    MatrixTransform() {}

    void doMAT_Identify(Matrix44* result);
    void doMAT_Multiply(Matrix44* result, Matrix44* srcA, Matrix44* srcB);

    /*---------------------Model matrix---------------------*/
    void doMAT_Rotate(Matrix44* result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void doMAT_Scale(Matrix44* result, GLfloat sx, GLfloat sy, GLfloat sz);
    void doMAT_Translate(Matrix44 * result, GLfloat x, GLfloat y, GLfloat z);

    /*---------------------View matrix---------------------*/
    void doMAT_LookAt(Matrix44* result,
                      const float eyeX, const float eyeY, const float eyeZ,
                      const float centerX, const float centerY, const float centerZ,
                      const float upX, const float upY, const float upZ);

    /*---------------------Normal matrix---------------------*/
    void doMat_ExtractMat3FromMat4(Matrix44* src, Matrix33* dest);

    /*---------------------Projection matrix---------------------*/
    void doMAT_OrthogonalProjection(Matrix44* result, GLfloat left, GLfloat right, GLfloat bottom,
                                    GLfloat top, GLfloat near, GLfloat far);

    void doMAT_LeftBottomAsOriginrojection(Matrix44* result, GLfloat width, GLfloat height);

    bool doMAT_PerspectiveProjection(Matrix44* result,
                                     const float fovy, const float aspect, const float zNear, const float zFar);

    /*---------------------Debug---------------------*/
    void doMAT_Dump(const Matrix44* M, const char * tag);

private:
    bool doMAT_Frustumf(Matrix44* result,
                        const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal);

};

}
#endif
