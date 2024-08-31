//
// Created by android_01 on 2024/8/28.
//

#include "Preview.h"

void* run(void* arg) {
    auto *preview = (Preview *)arg;
    preview->runDraw();

    LOGD("preview thread died");
    return NULL;
}

Preview::Preview(ANativeWindow *window) {
    lock = PTHREAD_MUTEX_INITIALIZER;
    cond = PTHREAD_COND_INITIALIZER;

    mWindow = window;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    if (pthread_create(&threadId, NULL, run, (void *)this) != 0) {
        LOGD("start preview thread fail");
        return;
    }
    LOGD("start preview thread success");
}

Preview::~Preview() {
    pthread_mutex_lock(&lock);
    event = EVENT::QUITE;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);

    pthread_join(threadId, NULL);
    threadId = NULL;
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    if (data) {
        free(data);
        data = NULL;
    }
}

void Preview::draw(uvc_frame *pFrame) {
    int ret;
    ret = pthread_mutex_trylock(&lock);
    if (ret != 0) {
        return;
    }

    event = EVENT::DRAW;
    if (data) {
        free(data);
        data = NULL;
    }
    width = pFrame->width;
    height = pFrame->height;
    data = malloc(pFrame->width * pFrame->height * 3);
    memcpy(data, pFrame->data, width * height * 3);

    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&cond);
}

void Preview::initEGL() {
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint width, height, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    surface = eglCreateWindowSurface(display, config, mWindow, NULL);
    EGLint attrs[]= {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    context = eglCreateContext(display, config, NULL, attrs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI(1, "------EGL-FALSE");
        return ;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &height);

    mDisplay = display;
    mSurface = surface;
    mContext = context;
    mWidth = width;
    mHeight = height;
//    LOGI(1, "width:%d, height:%d", mWidth, mHeight);
}

void Preview::terminateDisplay() {
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
        }
        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
        }
        eglTerminate(mDisplay);
    }

    mDisplay = EGL_NO_DISPLAY;
    mSurface = EGL_NO_SURFACE;
    mContext = EGL_NO_CONTEXT;
}

void Preview::runDraw() {
    initEGL();
    float vertices[] = {
            // ... 顶点数据
            -1, -1,
            1,  1,
            -1,  1,
            1,  1,
            -1, -1,
            1, -1,
    };
    float coordinate[] = {
            0,  1,
            1,  0,
            0,  0,
            1,  0,
            0,  1,
            1,  1
    };

    const char* vertexShaderSource =
            "attribute vec2 aPos;\n"
            "attribute vec2 aCoordinate;\n"
            "varying vec2 vCoordinate;\n"
            "uniform mat4 u_ProjMatrix;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = u_ProjMatrix * vec4(aPos, 0, 1.0);\n"
            "   vCoordinate = aCoordinate;\n"
            "}\n";
    const char* fragmentShaderSource =
            "precision mediump float;\n\n"
            "varying vec2 vCoordinate;\n"
            "uniform sampler2D uTexture;\n"
            "void main()\n"
            "{\n"
            "   gl_FragColor = texture2D(uTexture, vCoordinate);\n"
            "}\n";

    int program = GLUtil::createProgram(vertexShaderSource, fragmentShaderSource);

    GLint aPos = glGetAttribLocation(program, "aPos");
    GLint aCoordinate = glGetAttribLocation(program, "aCoordinate");
    GLint uTexture = glGetUniformLocation(program, "uTexture");
    GLint u_ProjMatrix = glGetUniformLocation(program, "u_ProjMatrix");

    int textureID = GLUtil::createTextures();
    glUseProgram(program);

    bool run = true;
    while (run) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&cond, &lock);

        switch (event) {
            case EVENT::DRAW:
                float ratioWidth;
                float ratioHeight;
                float r;
                float project[16];

                GLUtil::setIdentityM(project, 0);

                ratioWidth = mWidth / (float) width;
                ratioHeight = mHeight / (float) height;
                if (ratioWidth > ratioHeight) {
                    r = 1 / ratioWidth * ratioHeight;
                    GLUtil::orthoM(project, 0, 1, -1, -r, r, 1, -1);
                } else {
                    r = 1 / ratioHeight * ratioWidth;
                    GLUtil::orthoM(project, 0, -r, r, -1, 1, 1, -1);
                }

                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);

                glVertexAttribPointer(aPos, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(aPos);
                glVertexAttribPointer(aCoordinate, 2, GL_FLOAT, false, 0, coordinate);
                glEnableVertexAttribArray(aCoordinate);

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                glUniform1i(uTexture, 0);

                glUniformMatrix4fv(u_ProjMatrix, 1, false, project);

                glDrawArrays(GL_TRIANGLES, 0, 6);

                glDisableVertexAttribArray(aPos);
                glDisableVertexAttribArray(aCoordinate);
                glBindTexture(GL_TEXTURE_2D, 0);

                eglSwapBuffers(mDisplay, mSurface);

                break;
            case EVENT::QUITE:
                run = false;
                break;
        }
        pthread_mutex_unlock(&lock);
    }
    terminateDisplay();
}
