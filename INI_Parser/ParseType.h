#ifndef PARSETYPE_H_
#define PARSETYPE_H_

#include <string>

using std::string;

typedef struct
{
    unsigned int LayerIndex;
    string LayerRenderEffect;
    unsigned int LayerDrawTick;
    unsigned int LayerOrder;
    unsigned int LayerPositionX;
    unsigned int LayerPositionY;
    unsigned int LayerWidth;
    unsigned int LayerHeight;
    float LayerAlpha;
    string LayerObjectModel;
    string LayerTexture;
} LayerRenderType;

typedef struct
{
    unsigned int LayerRender;
    bool AndroidFLATLAND;
    bool GoogleTest;
} MainFunctionType;

#endif
