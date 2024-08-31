//
// Created by android_01 on 2024/8/27.
//

#ifndef LIBUVC_UVCCAMERA_H
#define LIBUVC_UVCCAMERA_H

#include "libuvc/libuvc.h"
#include "libuvc/libuvc_internal.h"
#include "libusb.h"
#include <stdio.h>
#include <unistd.h>
#include <jni.h>
#include <android/log.h>
#include "Preview.h"
#include <android/native_window_jni.h>
#include <string>
#include <vector>

#define TAG "uvc_camera"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

void cb(uvc_frame_t *frame, void *ptr);

struct UvcInfo {
    int width;
    int height;
    int interval;
    std::string intervals;
};

class UvcCamera {

    uvc_context_t *ctx = NULL;
    uvc_error_t res;
    uvc_device_handle_t *devh = NULL;
    uvc_device_t *dev = NULL;
    ANativeWindow *window = NULL;
    int width = 640;
    int height = 480;
    int interval = 25;

public:
    JavaVM *jvm;
    Preview *preview = NULL;
    jobject callback = NULL;
    jobject dataType = NULL;
    UvcCamera();
    ~UvcCamera();
    void open(int fd);
    void close();
    void startPreview();
    void stopPreview();
    void setPreview(ANativeWindow *window);
    void setDataCallback(jobject callback);
    std::vector<UvcInfo> fetchSupportSize();
    void setSize(int width, int height, int interval);
};

void handleFrame(uvc_frame_t *pFrame, UvcCamera *uvcCamera);

#endif //LIBUVC_UVCCAMERA_H
