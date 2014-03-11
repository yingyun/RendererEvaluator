/*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#ifndef  SHADERPROGRAMBUILDER_H_
#define  SHADERPROGRAMBUILDER_H_
#include <stdio.h>
#include <stdlib.h>
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

class ShaderProgramBuilder : public Singleton<ShaderProgramBuilder>
{
public:
    ShaderProgramBuilder() {}
    GLuint buildShaderProgram(const char* pVertexSource, const char* pFragmentSource);
    void useShaderProgram(GLuint program);

private:
    GLuint loadShaderProgram(GLenum shaderType, const char* pSource);
};

}

#endif
