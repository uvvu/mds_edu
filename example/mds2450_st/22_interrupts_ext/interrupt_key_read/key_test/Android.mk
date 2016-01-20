LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	key_test.c

#LOCAL_SHARED_LIBRARIES := \
#	libcutils               \
#	libutils                \
#	libui

LOCAL_MODULE := key_test

LOCAL_MODULE_TAGS := eng

LOCAL_C_INCLUDES := \
	frameworks/base/include \
	system/core/include

include $(BUILD_EXECUTABLE)
