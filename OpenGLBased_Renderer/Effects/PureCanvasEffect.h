/*
* 20140129 Cui.Yingyun
 */

#ifndef RENDEREVALUATOR_PURECANVASEFFECT_H_
#define RENDEREVALUATOR_PURECANVASEFFECT_H_

#include <utils/String8.h>
#include "EffectBase.h"
#include "../Logging.h"

using android::String8;

namespace RenderEvaluator
{

class PureCanvasEffect : public EffectBase
{
private:
    String8 mFragShader;
    String8 mVertexShader;
    int surfaceHeight;
    int surfaceWidth;
    Matrix44 projectionMatrix;

    GLfloat * vertexData;
    GLfloat * texCoordsData;
    GLuint texCoordsHandler;
    GLuint positionHandler;
    GLuint projectionHandler;
    GLuint samplerHandler;
    GLuint mProgram;
    GLuint texture[1];

public:
    PureCanvasEffect();
    virtual ~PureCanvasEffect() {}
    virtual bool updateShaderOnce();
    virtual bool updateAttributeOnce();
    virtual bool updateBufferOnce();
    virtual bool drawPolygonEvery();
    virtual bool updateFrameEvery();
};
}
#endif
