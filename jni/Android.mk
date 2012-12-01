LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include /opt/android-OpenCV-2.4.2/sdk/native/jni/OpenCV.mk


OPENCV_INSTALL_MODULES:=on
LOCAL_LDLIBS    += -llog -ldl -lGLESv2 -landroid
LOCAL_CFLAGS    := -Werror 
LOCAL_MODULE    := ar-jni
LOCAL_SRC_FILES := ar-jni.cpp arLib/gles2/Renderer.cpp arLib/model/Model.cpp arLib/ArLib.cpp arLib/EnvironmentData.cpp 
LOCAL_SRC_FILES += arLib/util/GeoMath.cpp
LOCAL_C_INCLUDES += /opt/glm-0.9.3.4/
include $(BUILD_SHARED_LIBRARY)
