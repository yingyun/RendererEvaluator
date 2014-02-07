/*
*20140129 Cui.Yingyun
*/
#include "TextureGenerator.h"
namespace RenderEvaluator
{
void TextureGenerator::loadTexture(int * width, int * height, void * * pixelData, SkBitmap& bitmap)
{

    /*
    *M.png was the PNG32 format under the /data/ directory.
    *Assume that we just support 8888 format for now and must be an NonOfPower size.
    *SkBitmap::kARGB_8888_Config:
    */

    //const char* fileName = "/data/M.png";
    //const char* fileName = "/data/DesertTreeCloud.png";   DarkModel-MiddBG.png
    //const char* fileName = "/data/MiddModel-MiddBG.png";
    const char* fileName = "/data/DarkModel-MiddBG.png";
    struct stat dest;
    if(stat(fileName, &dest) < 0)
        {
            LOG_ERROR("Get the texture file size failed!");
            exit(-1);
        }

    /* Address and alloc the memory for pixel */
    unsigned int fileSize =  dest.st_size;
    void * imagePixels = malloc(fileSize);
    if(imagePixels == NULL)
        {
            LOG_ERROR("imagePixels OOM!");
            exit(-1);
        }

    FILE * file = NULL;
    file = fopen(fileName, "r");
    if (file == NULL) LOG_ERROR("Open texture was faild!\n");
    unsigned int haveReaded = fread(imagePixels, 1, fileSize, file);
    if(fileSize == haveReaded) LOG_INFO("Load %d bytes texture data was sucessful!\n", haveReaded);
    fclose(file);

    /* Encode */
    SkMemoryStream stream(imagePixels, fileSize);
    SkImageDecoder* codec = SkImageDecoder::Factory(&stream);
    codec->setDitherImage(false);
    if(codec)
        {
            codec->decode(&stream, &bitmap, SkBitmap::kARGB_8888_Config, SkImageDecoder::kDecodePixels_Mode);
            delete codec;
            codec = NULL;
        }
    else
        {
            LOG_ERROR("Create codec was faild!\n");
        }

    *width = bitmap.width();
    *height = bitmap.height();
    *pixelData= bitmap.getPixels();
    free(imagePixels);

    int p2Width = 1 << (31 - __builtin_clz(*width));
    int p2Height = 1 << (31 - __builtin_clz(*height));
    LOG_INFO("Texture dimension: width %d, height %d\n", *width, *height);

    if(p2Width != *width || p2Height != *height)
        {
            LOG_ERROR("Non power of 2 size for Texture data!\n");
            /*
            *As the traditional OpenGL spec said,  Spec didn't support non-power of 2 texture size , but vairous implementation
            * can support it via his own extensions, So, we keep the maxium texture siza as screen size (e.g. 2013Nexus 7)
            */
            //exit(-1);
        }
}

void TextureGenerator::samplingMode(int minFilter, int magFilter, int sFilter, int tFilter)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tFilter);
}




}
