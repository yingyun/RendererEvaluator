LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	RenderingEvaluator.cpp \
	RenderMachine.cpp \
	NativeSurfaceFlingerEGLService.cpp \
	MatrixTransform.cpp \
	VertexGenerator.cpp \
	ShaderGen.cpp

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libEGL \
    libGLESv2 \
    libui \
    libgui \
    libbinder \
    libskia

LOCAL_MODULE:= RenderingEvalutor

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
