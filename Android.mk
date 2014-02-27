LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

BOOST_STATIC_VERSION := gcc-mt-1_55
BOOST_INCLUDE_PATH := external/boost_1_55_0/include
BOOST_STATIC_LIB_PATH := external/boost_1_55_0/static_lib

LOCAL_C_INCLUDES += \
	external/stlport/stlport \
	bionic \
	bionic/libstdc++/include \
	$(BOOST_INCLUDE_PATH)

LOCAL_SRC_FILES:= \
	Effects/GrayscaleEffect.cpp \
	Effects/PureCanvasEffect.cpp \
	INI_Parser/ini.cpp \
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

#LOCAL_STATIC_LIBRARIES := \
	$(BOOST_STATIC_LIB_PATH)/libboost_regex-$(BOOST_STATIC_VERSION).a

LOCAL_CFLAGS += -std=c++11

LOCAL_MODULE:= RenderEvaluator

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
