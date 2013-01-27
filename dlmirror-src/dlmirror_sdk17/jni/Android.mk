LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

INTERNAL_LIBS_PATH := $(LOCAL_PATH)/../android_lib
INTERNAL_INCLUDES_PATH := $(LOCAL_PATH)/../android_include

LOCAL_LDLIBS += -lbinder -lcutils -lutils -lui -lgui -L$(INTERNAL_LIBS_PATH)

LOCAL_STATIC_LIBRARIES := libusb

LOCAL_MODULE:= dlmirror

LOCAL_SRC_FILES:= \
 screencap_flinger.cpp \
 tubecable.c \
 helper.c \
 dlmirror.c

LOCAL_C_INCLUDES += \
 $(LOCAL_PATH)/libusb \
 $(INTERNAL_INCLUDES_PATH)

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
