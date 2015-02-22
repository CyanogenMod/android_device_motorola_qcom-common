LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := unlock-us-gsm.c
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin/
LOCAL_MODULE := unlock-us-gsm
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
