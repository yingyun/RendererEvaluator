LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	gl2_basic.cpp \
	gl2_basic_render.cpp \
	NativeSurfaceFlingerEGLService.cpp \
	MatrixTransform.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libEGL \
    libGLESv2 \
    libui \
    libgui \
    libbinder

LOCAL_C_INCLUDES += $(call include-path-for, opengl-tests-includes)

LOCAL_MODULE:= Native_gl2_basic

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DGL_GLEXT_PROTOTYPES

include $(BUILD_EXECUTABLE)
