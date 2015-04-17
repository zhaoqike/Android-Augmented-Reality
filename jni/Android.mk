LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include ../OpenCV-2.4.9-android-sdk/sdk/native/jni/OpenCV.mk



LOCAL_AR_DIR := AugmentedReality/AugmentedReality
LOCAL_SRC_FILES := AugmentedReality.cpp \
$(LOCAL_AR_DIR)/AbstractFeatureMatcher.cpp \
$(LOCAL_AR_DIR)/ARSystem.cpp \
$(LOCAL_AR_DIR)/CloudPoint.cpp \
$(LOCAL_AR_DIR)/CloudMap.cpp \
$(LOCAL_AR_DIR)/Common.cpp \
$(LOCAL_AR_DIR)/Distance.cpp \
$(LOCAL_AR_DIR)/FeatureExtractor.cpp \
$(LOCAL_AR_DIR)/FeatureMatcher.cpp \
$(LOCAL_AR_DIR)/FindCameraMatrices.cpp \
$(LOCAL_AR_DIR)/KeyFrame.cpp \
$(LOCAL_AR_DIR)/MapMaker.cpp \
$(LOCAL_AR_DIR)/MultiCameraDistance.cpp \
$(LOCAL_AR_DIR)/MultiCameraPnP.cpp \
$(LOCAL_AR_DIR)/MyKeyPoint.cpp \
$(LOCAL_AR_DIR)/OFFeatureMatcher.cpp \
$(LOCAL_AR_DIR)/PreProcessor.cpp \
$(LOCAL_AR_DIR)/Render.cpp \
$(LOCAL_AR_DIR)/RichFeatureMatcher.cpp \
$(LOCAL_AR_DIR)/SfMUpdateListener.cpp \
$(LOCAL_AR_DIR)/stdafx.cpp \
$(LOCAL_AR_DIR)/Timer.cpp \
$(LOCAL_AR_DIR)/Tracker.cpp \
$(LOCAL_AR_DIR)/Triangulation.cpp \
$(LOCAL_AR_DIR)/YamlStorage.cpp \
$(LOCAL_AR_DIR)/FBTime.cpp \
$(LOCAL_AR_DIR)/SBTime.cpp \
$(LOCAL_AR_DIR)/TBTime.cpp \
$(LOCAL_AR_DIR)/KLTTime.cpp 





LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += ./jni/SSBA/COLAMD/Include
LOCAL_C_INCLUDES += ./jni/SSBA/SuiteSparse_config
LOCAL_C_INCLUDES += ./jni/SSBA/.
LOCAL_C_INCLUDES += ./jni/.
LOCAL_C_INCLUDES += ./jni/AugmentedReality/AugmentedReality

LOCAL_CPPFLAGS  := -std=c++0x

LOCAL_LDLIBS     += -llog -ldl
LOCAL_MODULE    := AugmentedReality
include $(BUILD_SHARED_LIBRARY)





