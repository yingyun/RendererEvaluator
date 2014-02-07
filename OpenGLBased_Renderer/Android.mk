LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	Effects/GrayscaleEffect.cpp \
	INI_Parser/ini.c \
	INI_Parser/INIReader.cpp \
	RenderEvaluator.cpp \
	RenderMachine.cpp \
	Platform/Native_Android.cpp \
	MatrixTransform.cpp \
	VertexGenerator.cpp \
	TextureGenerator.cpp \
	RenderChecker.cpp \
	ShaderProgramBuilder.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libEGL \
    libGLESv2 \
    libui \
    libgui \
    libbinder \
    libskia \
	libstlport

LOCAL_C_INCLUDES += \
	external/stlport/stlport \
	bionic \
	bionic/libstdc++/include

LOCAL_CFLAGS += -std=c++11

LOCAL_MODULE:= RenderEvaluator

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
