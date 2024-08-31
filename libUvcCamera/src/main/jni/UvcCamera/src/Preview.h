//
// Created by android_01 on 2024/8/28.
//

#ifndef UVCCAMERA_PREVIEW_H
#define UVCCAMERA_PREVIEW_H

#include <jni.h>
#include <GLES/gl.h>
#include <GLES/egl.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h> // 用于sleep函数
#include "libuvc/libuvc.h"
#include "GLUtil.h"
#include <malloc.h>

#define TAG "uvc_camera"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

class Preview {

private:
    enum EVENT {
        DRAW,
        QUITE
    };
    enum EVENT event;

public:
    ANativeWindow *mWindow;
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    EGLint mWidth;
    EGLint mHeight;

    pthread_t threadId;

    pthread_mutex_t lock;
    pthread_cond_t cond;

    void *data = NULL;
    int width;
    int height;

    Preview(ANativeWindow *window);
    ~Preview();
    void draw(uvc_frame *pFrame);
    void initEGL();
    void terminateDisplay();
    void runDraw();
};

#endif //UVCCAMERA_PREVIEW_H
