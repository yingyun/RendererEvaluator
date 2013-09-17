LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	RenderingEvalutor.cpp \
	Main_Renderer.cpp \
	NativeSurfaceFlingerEGLService.cpp \
	MatrixTransform.cpp \
	VertexGenerator.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libEGL \
    libGLESv2 \
    libui \
    libgui \
    libbinder

LOCAL_MODULE:= RenderingEvalutor

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
