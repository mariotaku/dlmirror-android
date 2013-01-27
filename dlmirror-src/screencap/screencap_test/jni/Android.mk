LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

INTERNAL_LIBS_PATH := $(LOCAL_PATH)/../../libscreencap_sdk17/android_lib
SCREENCAP_LIB_PATH := $(LOCAL_PATH)/../
INTERNAL_INCLUDES_PATH := $(LOCAL_PATH)/../android_include

LOCAL_LDLIBS += -lbinder -lcutils -lutils -lui -lgui -lscreencap_fliger_sdk17 -L$(INTERNAL_LIBS_PATH) -L$(SCREENCAP_LIB_PATH)

LOCAL_SRC_FILES:= \
        screencap_test.c

LOCAL_MODULE:= screencap_test

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
