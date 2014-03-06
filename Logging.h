#ifndef ERRORPRINT_H
#define ERRORPRINT_H

#include <stdio.h>
#include <stdlib.h>
#include "RenderChecker.h"

enum
{
    DEBUG_LEVEL = 0xf,
    INFO_LEVEL = 0xe,
    WARNING_LEVEL = 0xc,
    ERROR_LEVEL = 0x8
};

#define LOG_LEVEL INFO_LEVEL
#ifdef LOG_LEVEL
#define LOG_DEBUG(...)      do{if(0x1 & LOG_LEVEL) printf(__VA_ARGS__);}while (0)
#define LOG_INFO(...)       do{if(0x2 & LOG_LEVEL) printf(__VA_ARGS__);}while (0)
#define LOG_WARNING(...)    do{if(0x4 & LOG_LEVEL) printf(__VA_ARGS__);}while (0)
#define LOG_ERROR(...)      do{if(0x8 & LOG_LEVEL) printf(__VA_ARGS__);}while (0)
#else
#define LOG_DEBUG
#define LOG_INFO
#define LOG_WARNING
#define LOG_ERROR
#endif

/* OpenGL */
#define GL_ERROR_CHECK(description)  do{RenderChecker::getInstance().checkGLErrors(description);}while(0)

#endif
