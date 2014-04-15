/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef  VECTORTRANSFORM_H
#define VECTORTRANSFORM_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Logging.h"
#include "Pattern/Singleton.h"

namespace RenderEvaluator
{

class VectorTransform : public Singleton<VectorTransform>
{
public:
    VectorTransform() {}
    inline float  doVEC3_Length(const float vector[3]);
    bool doVEC3_Normalize(float vector[3]);
    void doVEC3_Cross(float result[3], const float vector0[3], const float vector1[3]);
};

}
#endif
