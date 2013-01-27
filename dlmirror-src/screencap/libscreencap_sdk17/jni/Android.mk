LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

INTERNAL_LIBS_PATH := $(LOCAL_PATH)/../android_lib
INTERNAL_INCLUDES_PATH := $(LOCAL_PATH)/../android_include

LOCAL_LDLIBS += -lbinder -lcutils -lutils -lui -lgui -L$(INTERNAL_LIBS_PATH)

LOCAL_SRC_FILES:= \
        screencap_flinger_sdk17.cpp

LOCAL_MODULE:= screencap_fliger_sdk17

LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES += \
		$(INTERNAL_INCLUDES_PATH) \

include $(BUILD_SHARED_LIBRARY)
