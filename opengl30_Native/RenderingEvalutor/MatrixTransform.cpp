/*
*These affine translation matrix maybe has some bugs
*
*
*
*/




#include "MatrixTransform.h"

namespace android
{

//#define LOG_ENABLE
#ifdef LOG_ENABLE
#define LOG(...) printf(__VA_ARGS__)  //FixMe;  What does VA_ARGS mean?
#else
#define LOG(...)
#endif

/*
* Cui.YY  FixMe; Need to review equation
* Detaild equation refer 3D Math primer please
*/
#define PI  3.14159265


void MatrixTransform::matrixIndentity(Matrix44 * result)
{
    memset(result, 0x0, sizeof(Matrix44));
    result->m[0][0] = 1.0f;
    result->m[1][1] = 1.0f;
    result->m[2][2] = 1.0f;
    result->m[3][3] = 1.0f;
}

void MatrixTransform::matrixMultiply(Matrix44 *result, Matrix44 *srcA, Matrix44 *srcB)
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

//FixMe;  Wrong matrix ?
void MatrixTransform::matrixRotate(Matrix44 * result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
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

            matrixMultiply( result, &rotMat, result );
        }
}

//FixMe; Wrong Matrix?
void MatrixTransform::matrixTranslate(Matrix44 * result, GLfloat tx, GLfloat ty, GLfloat tz)
{
    result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
    result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
    result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
    result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

//FixMe; Wrong Matrix?
void MatrixTransform::matrixScale(Matrix44 * result, GLfloat sx, GLfloat sy, GLfloat sz)
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

/*FixMe; The essence of 2D array */
void MatrixTransform::matrixDump(const Matrix44 * mDumped, const char * tag)
{
    /*
    * Use type (* variable_name)[ROW_NUM]; to define the 2D array
    * in which means, for instance float a[2][5] define we have two what
    * type as float (* aa)[5] variable, it's also means float[5] type.
    */
    const GLfloat (* M)[4] = mDumped->m;
    LOG("%s\n " \
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
void MatrixTransform::vectorDump(const Vector4 * vDumped)
{
    const GLfloat * V = vDumped->v;
    printf("%f, %f, %f, %f\n", V[0], V[1], V[2], V[3]);
}

}
