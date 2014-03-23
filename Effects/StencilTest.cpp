/*
*Stencil buffer operation
*
*Copyright (C) 2013 - 2014 Cui. Yingyun
*This file is released under the GPL2
*
*void glStencilFunc(GLenum func, GLint ref, GLuint mask)
*void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
*void glStencilMask(GLuint mask)
*
*OpenGLES2.0 ProgrammingGuide. Chapter11-StencilBuffer Testing.
*Using the stencil buffer can be considered a two-step operation.The first
*step is to initialize the stencil buffer with the per-pixel masks, which is done
*by rendering geometry and specifying how the stencil buffer should be
*updated. The second is generally to use those values to control subsequent
*rendering into the color buffer. In both cases, you specify how the parameters
*are to be used in the stencil test.
*/
#include "StencilTest.h"


/*
*TODO: The render result is the corruption, i didn't figure out it yet...
*The issue just happend in second frame, Successed with first failed from second frame.
*/

namespace RenderEvaluator
{

GLfloat StencilTest::vVertices[] =
{
    // Quad #0
    -0.75f,  0.25f,  0.50f,
    -0.25f,  0.25f,  0.50f,
    -0.25f,  0.75f,  0.50f,
    -0.75f,  0.75f,  0.50f,
    // Quad #1
    0.25f,  0.25f,  0.90f,
    0.75f,  0.25f,  0.90f,
    0.75f,  0.75f,  0.90f,
    0.25f,  0.75f,  0.90f,
    // Quad #2
    -0.75f, -0.75f,  0.50f,
    -0.25f, -0.75f,  0.50f,
    -0.25f, -0.25f,  0.50f,
    -0.75f, -0.25f,  0.50f,
    // Quad #3
    0.25f, -0.75f,  0.50f,
    0.75f, -0.75f,  0.50f,
    0.75f, -0.25f,  0.50f,
    0.25f, -0.25f,  0.50f,
    // Big Quad
    -1.00f, -1.00f,  0.00f,
    1.00f, -1.00f,  0.00f,
    1.00f,  1.00f,  0.00f,
    -1.00f,  1.00f,  0.00f
};

GLubyte StencilTest::indices[][6] =
{
    // Quad #0
    {  0,  1,  2,  0,  2,  3 },
    // Quad #1
    {  4,  5,  6,  4,  6,  7 },
    // Quad #2
    {  8,  9, 10,  8, 10, 11 },
    // Quad #3
    { 12, 13, 14, 12, 14, 15 },
    // Big Quad
    { 16, 17, 18, 16, 18, 19 }
};

GLfloat  StencilTest::colors[NumTests][4] =
{
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f, 0.0f }
};

StencilTest::StencilTest(LayerRenderType layerInfo)
{
    mLayerInfo = layerInfo;
}

StencilTest::~StencilTest()
{
    glDeleteProgram(mProgram);
}

bool StencilTest::updateShaderOnce()
{
    /* Do the shader build*/
    const char * vertexShader = "\
         //PureCanvasEffect VertexShader\n\
         attribute vec4 a_position;\n\
         void main(void) {\n\
            gl_Position = a_position;\n\
        }\n";
    mVertexShader.append(vertexShader);

    const char * fragShader = "\
        //PureCanvasEffect Fragment\n\
        precision mediump float;\n\
        uniform vec4 u_color;\n\
        void main(void) {\n\
            gl_FragColor = u_color;\n\
        }\n";
    mFragShader.append(fragShader);

    mProgram = ShaderProgramBuilder::getInstance().buildShaderProgram(
        mVertexShader.string(), mFragShader.string());
    ShaderProgramBuilder::getInstance().useShaderProgram(mProgram);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepthf(0.75f);
    glClearStencil(0x1);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool StencilTest::updateAttributeOnce()
{
    /*Get the attribute from GLSL*/
    positionHandler = glGetAttribLocation(mProgram, "a_position");
    colorHandler = glGetUniformLocation(mProgram, "u_color");
    LOG_INFO("Render=> position %d, color%d\n", positionHandler, colorHandler);

    return true;
}

bool StencilTest::updateBufferOnce()
{
    return true;
}

bool StencilTest::drawPolygonEvery()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glVertexAttribPointer(positionHandler, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(positionHandler);

    // Test 0:
    //
    // Initialize upper-left region.  In this case, the
    //   stencil-buffer values will be replaced because the
    //   stencil test for the rendered pixels will fail the
    //   stencil test, which is
    //
    //        ref   mask   stencil  mask    --->Will failed
    //      ( 0x7 & 0x3 ) < ( 0x1 & 0x3 )
    //
    //   The value in the stencil buffer for these pixels will
    //   be ---> 0x7.
    //
    glStencilFunc( GL_LESS, 0x7, 0x3 );
    glStencilOp( GL_REPLACE, GL_DECR, GL_DECR );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices[0]);

    // Test 1:
    //
    // Initialize the upper-right region.  Here, we'll decrement
    //   the stencil-buffer values where the stencil test passes
    //   but the depth test fails.  The stencil test is
    //
    //        ref  mask    stencil  mask    --->Will pass
    //      ( 0x3 & 0x3 ) > ( 0x1 & 0x3 )
    //
    //    but where the geometry fails the depth test.  The
    //    stencil values for these pixels will be ---> 0x0.
    //
    glStencilFunc( GL_GREATER, 0x3, 0x3 );
    glStencilOp( GL_KEEP, GL_DECR, GL_KEEP );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices[1]);

    // Test 2:
    //
    // Initialize the lower-left region.  Here we'll increment
    //   (with saturation) the stencil value where both the
    //   stencil and depth tests pass.  The stencil test for
    //   these pixels will be
    //
    //        ref  mask     stencil  mask   --->Will pass
    //      ( 0x1 & 0x3 ) == ( 0x1 & 0x3 )
    //
    //   The stencil values for these pixels will be ---> 0x2.
    //
    glStencilFunc( GL_EQUAL, 0x1, 0x3 );
    glStencilOp( GL_KEEP, GL_INCR, GL_INCR );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices[2]);

    // Test 3:
    //
    // Finally, initialize the lower-right region.  We'll invert
    //   the stencil value where the stencil tests fails.  The
    //   stencil test for these pixels will be
    //
    //        ref   mask    stencil  mask   ---> Will failed
    //      ( 0x2 & 0x1 ) == ( 0x1 & 0x1 )
    //
    //   The stencil value here will be set to ~((2^s-1) & 0x1),0x1 means
    //   value which was stored in stencil buffer already.
    //   (with the 0x1 being from the stencil clear value),
    //   where 's' is the number of bits in the stencil buffer
    //
    glStencilFunc( GL_EQUAL, 0x2, 0x1 );
    glStencilOp( GL_INVERT, GL_KEEP, GL_KEEP );
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices[3]);

    GLint   numStencilBits;
    glGetIntegerv( GL_STENCIL_BITS, &numStencilBits );
    GLuint  stencilValues[NumTests] =
    {
        // Result of test 0
        0x7,
        // Result of test 1
        0x0,
        // Result of test 2
        0x2,
        // Result of test 3.  We need to fill this
        0xff
    };
    stencilValues[3] = ~(((1 << numStencilBits) - 1) & 0x1) & 0xff;

    // Use the stencil buffer for controlling where rendering will
    //   occur.  We diable writing to the stencil buffer so we
    //   can test against them without modifying the values we
    //   generated.
    glStencilMask( 0x0 );

    for ( int i = 0; i < NumTests; ++i )
        {
            glStencilFunc( GL_EQUAL, stencilValues[i], 0xff);
            glUniform4fv( colorHandler, 1, colors[i]);
            glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices[4]);
        }

    return true;
}

bool StencilTest::updateParamsEvery()
{
    return true;
}

}

