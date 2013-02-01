LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_STATIC_LIBRARIES := libusb

LOCAL_MODULE:= dldecrypt

LOCAL_SRC_FILES:= \
 dldecrypt.c \
 tubecable.c \

LOCAL_C_INCLUDES += \
 $(LOCAL_PATH)/libusb

LOCAL_C_FLAGS += -Wall

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
