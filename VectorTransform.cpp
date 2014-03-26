/*
*Vector transform implementation.
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/

#include "VectorTransform.h"

namespace RenderEvaluator
{

RENDEREVALUATOR_SINGLETON_STATIC_VAR_INIT(VectorTransform);

inline float  VectorTransform::doVEC3_Length(const float vector[3])
{
    return sqrtf(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}

bool VectorTransform::doVEC3_Normalize(float vector[3])
{
    float length = doVEC3_Length(vector);

    if (length == 0.0f)
        return false;

    for (int i = 0; i < 3; i++)
        {
            vector[i] /= length;
        }

    return true;
}

void VectorTransform::doVEC3_Cross(float result[3], const float vector0[3], const float vector1[3])
{
    float temp[3];
    temp[0] = vector0[1] * vector1[2] - vector0[2] * vector1[1];
    temp[1] = vector0[2] * vector1[0] - vector0[0] * vector1[2];
    temp[2] = vector0[0] * vector1[1] - vector0[1] * vector1[0];

    for (int i = 0; i < 3; i++)
        {
            result[i] = temp[i];
        }
}


}
