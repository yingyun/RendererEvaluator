/*
*The opengl shader program build routine
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*/
#include "ShaderProgramBuilder.h"
namespace RenderEvaluator
{
#define VERTEX_SHADER GL_VERTEX_SHADER
#define FRAGMENT_SHADER GL_FRAGMENT_SHADER

RENDEREVALUATOR_SINGLETON_STATIC_VAR_INIT(ShaderProgramBuilder);

void ShaderProgramBuilder::useShaderProgram(GLuint program)
{
    LOG_INFO("Render=> Use GL program %d\n", program);
    glUseProgram( program);
    GL_ERROR_CHECK("ShaderProgramBuilder:use GL program");
}

GLuint ShaderProgramBuilder::buildShaderProgram(const char * pVertexSource, const char * pFragmentSource)
{
    GLuint vertexShader = loadShaderProgram(VERTEX_SHADER, pVertexSource);
    if (!vertexShader)
        return false;
    LOG_INFO("Render=> Pass Build Vertex Shader\n");

    GLuint pixelShader = loadShaderProgram(FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader)
        return false;
    LOG_INFO("Render=> Pass Build Fragment Shader\n");

    GLuint program = glCreateProgram();
    GL_ERROR_CHECK("ShaderProgramBuilder:Create Program");
    if (program)
        {
            glAttachShader(program, vertexShader);
            glAttachShader(program, pixelShader);
            GL_ERROR_CHECK("ShaderProgramBuilder:Attach Program");

            glLinkProgram(program);
            GL_ERROR_CHECK("ShaderProgramBuilder:Link Program");

            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
            if (linkStatus != GL_TRUE)
                {
                    GLint bufLength = 0;
                    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                    if (bufLength)
                        {
                            char* buf = new char[bufLength];
                            if (buf)
                                {
                                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                                    LOG_ERROR("Could not link program:\n%s\n", buf);
                                    delete buf;
                                }
                        }

                    glDeleteProgram(program);
                    program = 0;
                }
        }
    return program;
}

GLuint ShaderProgramBuilder::loadShaderProgram(GLenum shaderType, const char * pSource)
{
    GLuint shader = glCreateShader(shaderType);
    GL_ERROR_CHECK("ShaderProgramBuilder:Create Shader");
    if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
                {
                    GLint infoLen = 0;
                    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                    if (infoLen)
                        {
                            GL_ERROR_CHECK("ShaderProgramBuilder:Failed on Compile Shader");
                            char* buf = new char[infoLen];
                            if (buf)
                                {
                                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                                    LOG_ERROR("Could not compile shader %d:\n%s\n", shaderType, buf);
                                    delete buf;
                                }
                            glDeleteShader(shader);
                            shader = 0;
                        }
                }
        }
    else
        {
            LOG_ERROR("Create shader error!\n");
        }
    return shader;
}

}
