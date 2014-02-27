#include <RenderChecker.h>

namespace RenderEvaluator
{
void RenderChecker::checkGLDriver(EGLDisplay display)
{
    /*
    * glGetString was used to get the implementation basic information
    */
    char* vender = (char*)glGetString(GL_VENDOR);
    char* version =(char*)glGetString(GL_VERSION);
    char* glrenderer =(char*)glGetString(GL_RENDERER);
    char* glsl_version =(char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    char* extensions =(char*)glGetString(GL_EXTENSIONS);
    LOG_INFO("OpenGL Vender: %s\n", vender);
    LOG_INFO("Version: %s\n", version);
    LOG_INFO("Renderer: %s\n", glrenderer);
    LOG_INFO("%s\n", glsl_version);

    int integer4[4] = {0, 0, 0 , 0};
    float float4[4] = {0.0, 0.0, 0.0, 0.0};
    bool bool4[4] = {0, 0, 0, 0};
    LOG_INFO("-----Current EGL Configuration-----\n");
    glGetIntegerv(GL_VIEWPORT, integer4);
    LOG_INFO("ViewPort: %d, %d, %d, %d\n", integer4[0], integer4[1], integer4[2], integer4[3]);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, integer4);
    LOG_INFO("Max ViewPort: %d, %d, %d, %d\n", integer4[0], integer4[1], integer4[2], integer4[3]);
    glGetFloatv(GL_DEPTH_RANGE, float4);
    LOG_INFO("Depth Range: (%f - %f)\n", float4[0], float4[1]);

    glGetIntegerv(GL_CULL_FACE_MODE, integer4);
    LOG_INFO("Cull face mode 0x%x\n", integer4[0]);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, integer4);
    LOG_INFO("Max texture size: %d\n", integer4[0]);
    glGetIntegerv(GL_RED_BITS, integer4);
    LOG_INFO("Red_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_GREEN_BITS, integer4);
    LOG_INFO("Green_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_BLUE_BITS, integer4);
    LOG_INFO("Blue_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_ALPHA_BITS, integer4);
    LOG_INFO("Alpha_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_DEPTH_BITS, integer4);
    LOG_INFO("Depth_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_DEPTH_BUFFER_BIT, integer4);
    LOG_INFO("Depthbuffer_Bits:%d   ", integer4[0]);
    glGetIntegerv(GL_STENCIL_BITS, integer4);
    LOG_INFO("Stencil_Bits:%d \n", integer4[0]);

    EGLint numOfConfigs = -1;
    eglGetConfigs(display, NULL, 0, &numOfConfigs);
    LOG_INFO("-----Number of EGL configs :%d-----\n", numOfConfigs);
    EGLConfig* configData = new EGLConfig[numOfConfigs];
    eglGetConfigs(display, configData, numOfConfigs, &numOfConfigs);
    for(int i = 0; i < numOfConfigs; i++)
        {
            //printEGLConfigInformation(configData[i]);
        }
}

void RenderChecker::checkEGLConfig(EGLConfig config, EGLDisplay display)
{
#define X(VAL) {VAL, #VAL}
    struct
    {
        EGLint attribute;
        const char* name;
    } names[] =
    {
        X(EGL_BUFFER_SIZE),
        X(EGL_RED_SIZE),
        X(EGL_GREEN_SIZE),
        X(EGL_BLUE_SIZE),
        X(EGL_ALPHA_SIZE),
        X(EGL_DEPTH_SIZE),
        X(EGL_STENCIL_SIZE),
        X(EGL_SAMPLES),
        X(EGL_SAMPLE_BUFFERS),
        X(EGL_SURFACE_TYPE),
        X(EGL_COLOR_BUFFER_TYPE),
        X(EGL_RENDERABLE_TYPE),
        X(EGL_CONFIG_CAVEAT),
        X(EGL_CONFIG_ID),
        X(EGL_LEVEL),
        X(EGL_MAX_PBUFFER_HEIGHT),
        X(EGL_MAX_PBUFFER_PIXELS),
        X(EGL_MAX_PBUFFER_WIDTH),
        X(EGL_NATIVE_RENDERABLE),
        X(EGL_NATIVE_VISUAL_ID),
        X(EGL_NATIVE_VISUAL_TYPE),
        X(EGL_TRANSPARENT_TYPE),
        X(EGL_TRANSPARENT_RED_VALUE),
        X(EGL_TRANSPARENT_GREEN_VALUE),
        X(EGL_TRANSPARENT_BLUE_VALUE),
        X(EGL_BIND_TO_TEXTURE_RGB),
        X(EGL_BIND_TO_TEXTURE_RGBA),
        X(EGL_MIN_SWAP_INTERVAL),
        X(EGL_MAX_SWAP_INTERVAL),
        X(EGL_LUMINANCE_SIZE),
        X(EGL_ALPHA_MASK_SIZE),
        X(EGL_CONFORMANT)
    };
#undef X
    LOG_INFO("-----Configuration we have choosed!-----\n");
    for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); j++)
        {
            EGLint value = -1;
            EGLint returnVal = eglGetConfigAttrib(display, config, names[j].attribute, &value);
            EGLint error = eglGetError();
            if (returnVal && error == EGL_SUCCESS)
                {
                    LOG_INFO(" %s: ", names[j].name);
                    LOG_INFO("%d (0x%x)\n", value, value);
                }
        }
    LOG_INFO("\n");
}

void RenderChecker::checkGLErrors()
{
    do
        {
            GLenum error = glGetError();
            if (error == GL_NO_ERROR)
                break;
            LOG_ERROR("GL error 0x%04x", int(error));
            if (error == GL_INVALID_ENUM) LOG_ERROR("GL ERROR:  invalid enumerant");
            if (error == GL_INVALID_VALUE) LOG_ERROR("GL ERROR: invalid value");
            if (error == GL_INVALID_OPERATION) LOG_ERROR("GL ERROR: invalid operation");
            if (error == GL_OUT_OF_MEMORY) LOG_ERROR("GL ERROR: out of memory");
        }
    while (true);
}

}
