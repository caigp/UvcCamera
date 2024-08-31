LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(LIBUSB_ROOT_ABS)
LOCAL_SHARED_LIBRARIES += libusb1.0

LOCAL_C_INCLUDES += $(LIBUVC_INCLUDES)
LOCAL_SHARED_LIBRARIES += uvc

# 指定模块名称
LOCAL_MODULE := uvc-camera

LOCAL_SRC_FILES := \
	src/UvcCamera.cpp \
	src/test.cpp \
	src/Preview.cpp \
	src/GLUtil.cpp

LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -lGLESv2
LOCAL_LDLIBS += -lEGL
LOCAL_LDLIBS += -landroid

include $(BUILD_SHARED_LIBRARY)  # 动态库