#include <jni.h>
#include "UvcCamera.h"

jobject getEnumInstance(JNIEnv *env, const char* className, const char* name, const char* sig) {
    jclass enumClass = env->FindClass(className);
    jfieldID fieldId = env->GetStaticFieldID(enumClass, name, sig);
    return env->GetStaticObjectField(enumClass, fieldId);
}

void setJavaPtr(JNIEnv *env, jobject thiz, jlong ptr) {
    jclass _jclass = env->GetObjectClass(thiz);
    jfieldID _jfieldID = env->GetFieldID(_jclass, "nativePtr", "J");
    env->SetLongField(thiz, _jfieldID, (jlong) ptr);
}

jlong getJavaPtr(JNIEnv *env, jobject thiz) {
    jclass _jclass = env->GetObjectClass(thiz);
    jfieldID _jfieldID = env->GetFieldID(_jclass, "nativePtr", "J");
    return env->GetLongField(thiz, _jfieldID);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_open(JNIEnv *env, jobject thiz, jint fd) {
    auto *uvc = new UvcCamera();
    uvc->open(fd);

    env->GetJavaVM(&uvc->jvm);
    setJavaPtr(env, thiz, (jlong) uvc);
    uvc->dataType = env->NewGlobalRef(getEnumInstance(env, "com/xiaocai/app/DataType", "RGB", "Lcom/xiaocai/app/DataType;"));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera__1close(JNIEnv *env, jobject thiz) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->close();

        if (uvc->callback) {
            env->DeleteGlobalRef(uvc->callback);
            uvc->callback = NULL;
        }

        if (uvc->dataType) {
            env->DeleteGlobalRef(uvc->dataType);
            uvc->dataType = NULL;
        }

//        uvc->jvm->DestroyJavaVM();

        delete uvc;
        setJavaPtr(env, thiz, 0);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_startPreview(JNIEnv *env, jobject thiz) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->startPreview();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_stopPreview(JNIEnv *env, jobject thiz) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->stopPreview();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_setPreview(JNIEnv *env, jobject thiz, jobject surface) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        ANativeWindow *window = NULL;
        if (surface) {
            window = ANativeWindow_fromSurface(env, surface);
        }
        uvc->setPreview(window);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_setDataCallback(JNIEnv *env, jobject thiz, jobject callback) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->setDataCallback(env->NewGlobalRef(callback));
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_xiaocai_app_UvcCamera_getSupportSize(JNIEnv *env, jobject thiz) {
    jlong ptr = getJavaPtr(env, thiz);

    jclass list_jclass = env->FindClass("java/util/ArrayList");
    jmethodID jmethodId = env->GetMethodID(list_jclass, "<init>", "()V");
    jobject list = env->NewObject(list_jclass, jmethodId);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;

        std::vector<UvcInfo> sizes = uvc->fetchSupportSize();
        for (const auto &item: sizes) {
            jclass _jclass = env->FindClass("com/xiaocai/app/Size");
            jmethodId = env->GetMethodID(_jclass, "<init>", "(IIILjava/lang/String;)V");

            jstring str = env->NewStringUTF(item.intervals.c_str());
            jobject _size = env->NewObject(_jclass, jmethodId, ((int) item.width), ((int) item.height), item.interval, str);

            jmethodId = env->GetMethodID(list_jclass, "add", "(Ljava/lang/Object;)Z");
            env->CallBooleanMethod(list, jmethodId, _size);
        }
    }
    return list;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_setSize(JNIEnv *env, jobject thiz, jint width, jint height,
                                       jint interval) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->setSize(width, height, interval);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_xiaocai_app_UvcCamera_setPreviewMirror(JNIEnv *env, jobject thiz, jboolean mirror) {
    jlong ptr = getJavaPtr(env, thiz);
    if (ptr != 0) {
        auto *uvc = (UvcCamera *) ptr;
        uvc->setPreviewMirror(mirror);
    }
}