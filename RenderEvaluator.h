
#ifndef  RENDEREVALUATOR_H_
#define RENDEREVALUATOR_H_

#include "Platform/Native_Android.h"
#include "Logging.h"
#include "RenderMachine.h"
#include "INI_Parser/INIReader.h"
#include "INI_Parser/ParseType.h"

#include <string>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

#define NUMOFSURFACE_MAX 12
#define  CLAMP(numOfLayer) do{ if(numOfLayer< 1) numOfLayer = 1; else if(numOfLayer > NUMOFSURFACE_MAX) numOfLayer = NUMOFSURFACE_MAX;} while( 0 )
#define CONFIG_INI_PATH "/data/RenderEvaluator/config.ini"

using android::sp;
using RenderEvaluator::RenderMachine;
using RenderEvaluator::Native_Android;
using std::string;
using std::vector;

void usageRender();
LayerRenderType parseLayer(INIReader& configParser, string& layerName);
MainFunctionType parseMainFunction(INIReader& configParser);
void handleLayerRenderTask(INIReader& configParser, unsigned int numOfLayer);
void handleFlatlandTask(INIReader& configParser);
void handleGTest(INIReader& configParser);

#endif
