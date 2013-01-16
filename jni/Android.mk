LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE:= tubecable

LOCAL_SRC_FILES:= \
 descriptors.c \
 error.c \
 linux.c \
 usb.c \
 tubecable.cpp \
 helper.cpp \
 tubecable_demo.cpp

include $(BUILD_EXECUTABLE)
