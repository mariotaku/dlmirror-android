LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= libusb

LOCAL_C_INCLUDES += \
 os

LOCAL_SRC_FILES:= \
 core.c \
 descriptor.c \
 io.c \
 sync.c \
 compat.c \
 os/linux_usbfs.c \
 os/threads_posix.c

include $(BUILD_STATIC_LIBRARY)
