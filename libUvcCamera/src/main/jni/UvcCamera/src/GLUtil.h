//
// Created by weiersyuan on 2016/11/24.
//

#ifndef NATIVEGLESVIEW_GLUTIL_H
#define NATIVEGLESVIEW_GLUTIL_H

#include <GLES2/gl2.h>
#include <android/log.h>
#include <math.h>
#define LOGI(level, ...) __android_log_print(ANDROID_LOG_INFO, "NATIVE_LOG", __VA_ARGS__)
class GLUtil {

public:
    static int compileShader(int type, const char* shaderCode);
    static int createProgram(const char * vertexShaderCode, const char * fragmentShaderCode);
    static int createTextures();
    static void setIdentityM(float *sm, int smOffset);
    static void orthoM(float *m, int mOffset,
                       float left, float right, float bottom, float top,
                       float near, float far);
    static void setRotateM(float *rm, int rmOffset,
                           float a, float x, float y, float z);
    static float length(float x, float y, float z);
};


#endif //NATIVEGLESVIEW_GLUTIL_H
