LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LIBUSB_ROOT_ABS)
LOCAL_SHARED_LIBRARIES += libusb1.0

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libjpeg-turbo
LOCAL_LDLIBS += $(LOCAL_PATH)/../../jniLibs/$(TARGET_ARCH_ABI)/libturbojpeg.so

LIBUVC_INCLUDES := $(LOCAL_PATH)/include

# 指定模块名称
LOCAL_MODULE := uvc

LOCAL_SRC_FILES := \
	src/ctrl.c \
	src/ctrl-gen.c \
	src/device.c \
	src/diag.c \
	src/frame.c \
	src/frame-mjpeg2.c \
	src/init.c \
	src/stream.c \
	src/misc.c

LOCAL_C_INCLUDES += $(LIBUVC_INCLUDES)

include $(BUILD_SHARED_LIBRARY)  # 动态库