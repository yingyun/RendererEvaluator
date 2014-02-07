/*
 * 20140204 Cui.Yingyun
 */

#ifndef  SHADERPROGRAMBUILDER_H_
#define  SHADERPROGRAMBUILDER_H_
#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "Logging.h"

namespace RenderEvaluator
{

class ShaderProgramBuilder
{
public:
    static GLuint buildShaderProgram(const char* pVertexSource, const char* pFragmentSource);
    static void useShaderProgram(GLuint program);

private:
    static GLuint loadShaderProgram(GLenum shaderType, const char* pSource);
};

}

#endif
