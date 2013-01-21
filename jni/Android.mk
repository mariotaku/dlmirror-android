LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libusb

LOCAL_MODULE:= tubecable

LOCAL_SRC_FILES:= \
 tubecable.c \
 helper.c \
 android_screencap.c \
 tubecable_demo.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/libusb

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
