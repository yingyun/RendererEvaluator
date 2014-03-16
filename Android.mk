LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

BOOST_STATIC_VERSION := gcc-mt-1_53
BOOST_INCLUDE_PATH := external/boost_1_53_0/include
BOOST_STATIC_LIB_PATH := external/boost_1_53_0/static_lib
PREBUILT_STDCXX_PATH := prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++

LOCAL_C_INCLUDES += \
	external/stlport/stlport \
	bionic \
	bionic/libstdc++/include \
	$(BOOST_INCLUDE_PATH) \
	$(PREBUILT_STDCXX_PATH)/include \
	$(PREBUILT_STDCXX_PATH)/libs/$(TARGET_CPU_ABI)/include

LOCAL_SRC_FILES:= \
	Effects/GrayscaleEffect.cpp \
	Effects/PureCanvasEffect.cpp \
	Effects/EdgeDetection.cpp \
	Effects/GaussianBlur.cpp \
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

LOCAL_LDFLAGS := \
	-L$(PREBUILT_STDCXX_PATH)/libs/$(TARGET_CPU_ABI) \
	-lgnustl_static \
	-lsupc++ \

LOCAL_CPPFLAGS += -fexceptions -std=c++11

#OpenGL Switcher. In general, ES30 backward-compatibility with ES20
#but some of device didn't support ES30.
LOCAL_CFLAGS := -DUSE_OPENGL_ES_30
#LOCAL_CFLAGS := -DUSE_OPENGL_ES_20

LOCAL_MODULE:= RenderEvaluator

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
