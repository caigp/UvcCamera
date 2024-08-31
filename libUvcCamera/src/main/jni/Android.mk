PROJECT_PATH := $(call my-dir)

include $(CLEAR_VARS)

include $(PROJECT_PATH)/libusb/android/jni/libusb.mk
include $(PROJECT_PATH)/libuvc/libuvc.mk
include $(PROJECT_PATH)/UvcCamera/Android.mk