#ifndef  SHADERPROGRAMBUILDER_H_
#define  SHADERPROGRAMBUILDER_H_
#include <stdio.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

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
