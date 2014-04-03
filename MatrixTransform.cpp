/*
*Matrix transform implementation.
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/

#include "MatrixTransform.h"

namespace RenderEvaluator
{
/*
* Cui.YY  FixMe; Need to review equation
* Detaild equation refer 3D Math primer please
* Take a look at in Android matrix design
*/
#define PI  3.14159265

RENDEREVALUATOR_SINGLETON_STATIC_VAR_INIT(MatrixTransform);



void MatrixTransform::doMAT_Identify(Matrix44 * result)
{
    memset(result, 0x0, sizeof(Matrix44));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void MatrixTransform::doMAT_Multiply(Matrix44 *result, Matrix44 *srcA, Matrix44 *srcB)
{
    Matrix44 tmp;
    int i;
    for (i=0; i<4; i++)
        {
            tmp.m[i][0] = (srcA->m[i][0] * srcB->m[0][0]) + (srcA->m[i][1] * srcB->m[1][0])
                          + (srcA->m[i][2] * srcB->m[2][0]) + (srcA->m[i][3] * srcB->m[3][0]) ;

            tmp.m[i][1] = (srcA->m[i][0] * srcB->m[0][1]) + (srcA->m[i][1] * srcB->m[1][1])
                          + (srcA->m[i][2] * srcB->m[2][1]) + (srcA->m[i][3] * srcB->m[3][1]) ;

            tmp.m[i][2] = (srcA->m[i][0] * srcB->m[0][2]) + (srcA->m[i][1] * srcB->m[1][2])
                          + (srcA->m[i][2] * srcB->m[2][2]) + (srcA->m[i][3] * srcB->m[3][2]);

            tmp.m[i][3] = (srcA->m[i][0] * srcB->m[0][3]) + (srcA->m[i][1] * srcB->m[1][3])
                          + (srcA->m[i][2] * srcB->m[2][3]) + (srcA->m[i][3] * srcB->m[3][3]);
        }
    memcpy(result, &tmp, sizeof(Matrix44));
}

/*---------------------Model-Linear Transformation---------------------*/

void MatrixTransform::doMAT_Rotate(Matrix44 * result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat sinAngle;
    GLfloat cosAngle;
    GLfloat mag = sqrtf(x * x + y * y + z * z);
    GLfloat pass = (angle * PI)/180.0f;

    sinAngle = sinf(pass);
    cosAngle = cosf(pass);
    if ( mag > 0.0f )
        {
            GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
            GLfloat oneMinusCos;
            Matrix44 rotMat;

            x /= mag;
            y /= mag;
            z /= mag;

            xx = x * x;
            yy = y * y;
            zz = z * z;
            xy = x * y;
            yz = y * z;
            zx = z * x;
            xs = x * sinAngle;
            ys = y * sinAngle;
            zs = z * sinAngle;
            oneMinusCos = 1.0f - cosAngle;

            rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
            rotMat.m[0][1] = (oneMinusCos * xy) - zs;
            rotMat.m[0][2] = (oneMinusCos * zx) + ys;
            rotMat.m[0][3] = 0.0F;

            rotMat.m[1][0] = (oneMinusCos * xy) + zs;
            rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
            rotMat.m[1][2] = (oneMinusCos * yz) - xs;
            rotMat.m[1][3] = 0.0F;

            rotMat.m[2][0] = (oneMinusCos * zx) - ys;
            rotMat.m[2][1] = (oneMinusCos * yz) + xs;
            rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
            rotMat.m[2][3] = 0.0F;

            rotMat.m[3][0] = 0.0F;
            rotMat.m[3][1] = 0.0F;
            rotMat.m[3][2] = 0.0F;
            rotMat.m[3][3] = 1.0F;

            doMAT_Multiply( result, &rotMat, result );
        }
}

void MatrixTransform::doMAT_Scale(Matrix44 * result, GLfloat sx, GLfloat sy, GLfloat sz)
{
    result->m[0][0] *= sx;
    result->m[0][1] *= sx;
    result->m[0][2] *= sx;
    result->m[0][3] *= sx;

    result->m[1][0] *= sy;
    result->m[1][1] *= sy;
    result->m[1][2] *= sy;
    result->m[1][3] *= sy;

    result->m[2][0] *= sz;
    result->m[2][1] *= sz;
    result->m[2][2] *= sz;
    result->m[2][3] *= sz;
}

/*---------------------Model-Move origin---------------------*/

void MatrixTransform::doMAT_Translate(Matrix44 * result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}


/*---------------------View matrix---------------------*/

void MatrixTransform::doMAT_LookAt(Matrix44 * result,
                                   const float eyeX, const float eyeY, const float eyeZ,
                                   const float centerX, const float centerY, const float centerZ,
                                   const float upX, const float upY, const float upZ)
{
    float forward[3], side[3], up[3];

    forward[0] = centerX - eyeX;
    forward[1] = centerY - eyeY;
    forward[2] = centerZ - eyeZ;

    VectorTransform::getInstance().doVEC3_Normalize(forward);

    up[0] = upX;
    up[1] = upY;
    up[2] = upZ;

    VectorTransform::getInstance().doVEC3_Cross(side, forward, up);
    VectorTransform::getInstance().doVEC3_Normalize(side);
    VectorTransform::getInstance().doVEC3_Cross(up, side, forward);

    result->m[0][0] = side[0];
    result->m[0][1] = up[0];
    result->m[0][2] = -forward[0];
    result->m[0][3] = 0.0f;

    result->m[1][0] = side[1];
    result->m[1][1] = up[1];
    result->m[1][2] = -forward[1];
    result->m[1][3] = 0.0f;

    result->m[2][0] = side[2];
    result->m[2][1] = up[2];
    result->m[2][2] = -forward[2];
    result->m[2][3] = 0.0f;

    result->m[3][0] = 0.0f;
    result->m[3][1] = 0.0f;
    result->m[3][2] = 0.0f;
    result->m[3][3] = 1.0f;

    Matrix44 temp;
    doMAT_Identify(&temp);

    temp.m[3][0] = -eyeX;
    temp.m[3][1] = -eyeY;
    temp.m[3][2] = -eyeZ;

    doMAT_Multiply(result, result, &temp);
}

/*---------------------Projection matrix---------------------*/

/*
*Convenience method to setup android style ortho projection in which the origin point was
*the left-bottom and pixel based window size
*/
void MatrixTransform::doMAT_LeftBottomAsOriginrojection(Matrix44 * result, GLfloat width, GLfloat height)
{
    doMAT_OrthogonalProjection(result, 0, width, 0, height, 0, 1);
}

/*Orthoprojection matrix*/
void MatrixTransform::doMAT_OrthogonalProjection(Matrix44 * result, GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    result->m[0][0] =  2 / (right - left);
    result->m[1][1] =  2 / (top   - bottom);
    result->m[2][2] = -2 / (far   - near);
    result->m[3][0] = -(right + left)   / (right - left);
    result->m[3][1] = -(top   + bottom) / (top   - bottom);
    result->m[3][2] = -(far   + near)   / (far   - near);
}

/*Perspective projection*/
bool MatrixTransform::doMAT_Frustumf(Matrix44* result,
                                     const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal)
{
    if ((right - left) == 0.0f || (top - bottom) == 0.0f || (farVal - nearVal) == 0.0f)
        {
            return false;
        }

    result->m[0][0] = 2.0f * nearVal / (right - left);
    result->m[0][1] = 0.0f;
    result->m[0][2] = 0.0f;
    result->m[0][3] = 0.0f;
    result->m[1][0] = 0.0f;
    result->m[1][1] = 2.0f * nearVal / (top - bottom);
    result->m[1][2] = 0.0f;
    result->m[1][3] = 0.0f;
    result->m[2][0] = (right + left) / (right - left);
    result->m[2][1] = (top + bottom) / (top - bottom);
    result->m[2][2] = -(farVal + nearVal) / (farVal - nearVal);
    result->m[2][3] = -1.0f;
    result->m[3][0] = 0.0f;
    result->m[3][1] = 0.0f;
    result->m[3][2] = -(2.0f * farVal * nearVal) / (farVal - nearVal);
    result->m[3][3] = 0.0f;

    return true;
}

bool MatrixTransform::doMAT_PerspectiveProjection(Matrix44* result,
        const float fovy, const float aspect, const float zNear, const float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * tanf(fovy * PI / 360.0f);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    return doMAT_Frustumf(result, xmin, xmax, ymin, ymax, zNear, zFar);
}

/*---------------------Debug---------------------*/
void MatrixTransform::doMAT_Dump(const Matrix44 * mDumped, const char * tag)
{
    /*
    * Use type (* variable_name)[ROW_NUM]; to define the 2D array
    * in which means, for instance float a[2][5] define we have two what
    * type as float (* aa)[5] variable, it's also means float[5] type.
    */
    const GLfloat (* M)[4] = mDumped->m;
    LOG_DEBUG("%s\n " \
              "\t %f %f %f %f\n" \
              "\t %f %f %f %f\n" \
              "\t %f %f %f %f\n"\
              "\t %f %f %f %f\n"
              ,tag
              ,M[0][0] ,M[0][1] ,M[0][2] ,M[0][3]
              ,M[1][0] ,M[1][1] ,M[1][2] ,M[1][3]
              ,M[2][0] ,M[2][1] ,M[2][2] ,M[2][3]
              ,M[3][0] ,M[3][1] ,M[3][2] ,M[3][3]
             );
}

}
