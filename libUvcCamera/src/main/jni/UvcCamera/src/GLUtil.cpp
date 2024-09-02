//
// Created by weiersyuan on 2016/11/24.
//

#include "GLUtil.h"
int GLUtil::compileShader(int type, const char * shaderCode) {

    int shader = glCreateShader(type);
    if (shader == 0) {

    }
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    GLint compileStatus = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        if (!compileStatus) {
            glDeleteShader(shader);
            LOGI(1, "compile shader error");
            return 0;
        }
        return shader;
}

int GLUtil::createProgram(const char *vertexShaderCode, const char *fragmentShaderCode) {
    GLint program = glCreateProgram();
    if (0 == program) {
        LOGI(1, "create program error");
        return 0;
    }
    LOGI(1, "create program success");
    int vertexShaderID = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
    int fragmentShaderID = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    glAttachShader(program, vertexShaderID);
    glAttachShader(program, fragmentShaderID);
    glLinkProgram(program);
    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (0 == linkStatus) {
        glDeleteProgram(program);
        LOGI(1, "link program error");
        return 0;
    }
    return program;

}

int GLUtil::createTextures() {

    GLuint textureID;
    glGenTextures(1, &textureID);
//    glBindTexture(GL_TEXTURE_2D, textureID);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    // 设置纹理参数（如过滤和包装）
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

void GLUtil::setIdentityM(float *sm, int smOffset) {
    for (int i=0 ; i<16 ; i++) {
        sm[i] = 0;
    }
    for(int i = 0; i < 16; i += 5) {
        sm[smOffset + i] = 1.0f;
    }
}

void
GLUtil::orthoM(float *m, int mOffset, float left, float right, float bottom, float top, float near,
               float far) {
    float r_width  = 1.0f / (right - left);
    float r_height = 1.0f / (top - bottom);
    float r_depth  = 1.0f / (far - near);
    float x =  2.0f * (r_width);
    float y =  2.0f * (r_height);
    float z = -2.0f * (r_depth);
    float tx = -(right + left) * r_width;
    float ty = -(top + bottom) * r_height;
    float tz = -(far + near) * r_depth;
    m[mOffset + 0] = x;
    m[mOffset + 5] = y;
    m[mOffset +10] = z;
    m[mOffset +12] = tx;
    m[mOffset +13] = ty;
    m[mOffset +14] = tz;
    m[mOffset +15] = 1.0f;
    m[mOffset + 1] = 0.0f;
    m[mOffset + 2] = 0.0f;
    m[mOffset + 3] = 0.0f;
    m[mOffset + 4] = 0.0f;
    m[mOffset + 6] = 0.0f;
    m[mOffset + 7] = 0.0f;
    m[mOffset + 8] = 0.0f;
    m[mOffset + 9] = 0.0f;
    m[mOffset + 11] = 0.0f;
}

void GLUtil::setRotateM(float *rm, int rmOffset, float a, float x, float y, float z) {
    rm[rmOffset + 3] = 0;
    rm[rmOffset + 7] = 0;
    rm[rmOffset + 11]= 0;
    rm[rmOffset + 12]= 0;
    rm[rmOffset + 13]= 0;
    rm[rmOffset + 14]= 0;
    rm[rmOffset + 15]= 1;
    a *= (float) (M_PI / 180.0f);

    float s = (float) sin(a);
    float c = (float) cos(a);
    if (1.0f == x && 0.0f == y && 0.0f == z) {
        rm[rmOffset + 5] = c;   rm[rmOffset + 10]= c;
        rm[rmOffset + 6] = s;   rm[rmOffset + 9] = -s;
        rm[rmOffset + 1] = 0;   rm[rmOffset + 2] = 0;
        rm[rmOffset + 4] = 0;   rm[rmOffset + 8] = 0;
        rm[rmOffset + 0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        rm[rmOffset + 0] = c;   rm[rmOffset + 10]= c;
        rm[rmOffset + 8] = s;   rm[rmOffset + 2] = -s;
        rm[rmOffset + 1] = 0;   rm[rmOffset + 4] = 0;
        rm[rmOffset + 6] = 0;   rm[rmOffset + 9] = 0;
        rm[rmOffset + 5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        rm[rmOffset + 0] = c;   rm[rmOffset + 5] = c;
        rm[rmOffset + 1] = s;   rm[rmOffset + 4] = -s;
        rm[rmOffset + 2] = 0;   rm[rmOffset + 6] = 0;
        rm[rmOffset + 8] = 0;   rm[rmOffset + 9] = 0;
        rm[rmOffset + 10]= 1;
    } else {
        float len = length(x, y, z);
        if (1.0f != len) {
            float recipLen = 1.0f / len;
            x *= recipLen;
            y *= recipLen;
            z *= recipLen;
        }
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        rm[rmOffset +  0] = x*x*nc +  c;
        rm[rmOffset +  4] =  xy*nc - zs;
        rm[rmOffset +  8] =  zx*nc + ys;
        rm[rmOffset +  1] =  xy*nc + zs;
        rm[rmOffset +  5] = y*y*nc +  c;
        rm[rmOffset +  9] =  yz*nc - xs;
        rm[rmOffset +  2] =  zx*nc - ys;
        rm[rmOffset +  6] =  yz*nc + xs;
        rm[rmOffset + 10] = z*z*nc +  c;
    }
}

float GLUtil::length(float x, float y, float z)  {
    return (float) sqrt(x * x + y * y + z * z);
}
