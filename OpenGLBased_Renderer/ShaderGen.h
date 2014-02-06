/*
* 2014 Cui.YY(cuiyingyun@gmail.com)
* These constructor idea get from Android HWUI design
* Some case will result bad performance of shader as the lack of  good hand coding
*/

#ifndef SHADERGEN_H
#define SHADERGEN_H

namespace android
{

class ShaderGen
{
public:
    ShaderGen()
    {
        //Do something in the future
    }

    //Shader for vertex
    static const char * gVS_Header_Attribute_vertexPosition;  //Header
    static const char * gVS_Header_Uniform_rotationMatrix;
    static const char * gVS_Header_Uniform_scaleMatrix;
    static const char * gVS_Header_Uniform_translationMatrix;
    static const char * gVS_Header_Uniform_OrthoProjcMatrix;
    static const char * gVS_Header_Attribute_passColor;
    static const char * gVS_Header_Attribute_texCoord;
    static const char * gVS_Header_Varying_colorToFrag;
    static const char * gVS_Header_Varying_texCoordToFrag;

    static const char * gVS_Main_Start_Function;  //Body
    static const char * gVS_Function_Direct_Pass_Position;
    static const char * gVS_Function_Pass_RO_Multi_Position;
    static const char * gVS_Function_Pass_SC_Multi_Position;
    static const char * gVS_Function_Pass_TR_Multi_Position;
    static const char * gVS_Function_OrthoProjection_Multi_Position;
    static const char * gVS_Function_Pass_Color_To_Frag;
    static const char * gVS_Function_Pass_texCoord_To_Frag;
    static const char * gVS_Function_Gaussian_Blur;
    static const char * gVS_Main_End_Function;

    //Shader for fragment
    static const char * gFS_Header_Precision_Mediump_Float;  //Header
    static const char * gFS_Header_Varying_colorToFrag;
    static const char * gFS_Header_Varying_texCoordToFrag;
    static const char * gFS_Header_Sampler2D;
    static const char * gFS_Header_Brightness_Alpha;

    static const char * gFS_Main_Start_Function;  //Body
    static const char * gFS_Function_Pass_Constant_Color;
    static const char * gFS_Function_Direct_Pass_Color;
    static const char * gFS_Function_Direct_Sampler_texCoord;
    static const char * gFS_Function_Gaussian_Blur;
    static const char * gFS_Function_Brightness;
    static const char * gFS_Function_Grayscale;
    static const char * gFS_Main_End_Function;
};

}//Namespace of android
#endif
