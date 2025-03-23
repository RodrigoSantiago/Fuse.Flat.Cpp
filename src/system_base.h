//
// Created by Rodrigo on 21/03/2025.
//

#ifndef FLAT_SYSTEM_BASE_H
#define FLAT_SYSTEM_BASE_H

#include <jni.h>
#include <iostream>
#include <GLFW/glfw3.h>

JNIEnv* getJNIEnv();
void releaseJNIEnv();

template<class T>
class jLambda;

template<class R, class ...Args> class jLambda<R(Args...)> {
public:
    jobject obj;
    jmethodID method;

    jLambda() : obj(nullptr), method (nullptr) {

    }

    jLambda(jobject jobj, jmethodID jmth) : obj(nullptr), method (nullptr) {
        set(jobj, jmth);
    }

    jLambda(const jLambda<R(Args...)>& other) : obj(nullptr), method(nullptr) {
        set(other.obj, other.method);
    }

    jLambda<R(Args...)>& operator=(std::nullptr_t) {
        set(nullptr, nullptr);
        return *this;
    }

    jLambda<R(Args...)>& operator=(jLambda<R(Args...)> other) {
        set(other.obj, other.method);
        return *this;
    }

    void run(Args... args) const {
        getJNIEnv()->CallVoidMethod(obj, method, args...);
    }

    R bRun(Args... args) {
        return getJNIEnv()->CallBooleanMethod(obj, method, args...);
    }

    void set(jobject jobj, jmethodID jmth) {
        if (obj != nullptr) {
            getJNIEnv()->DeleteGlobalRef(obj);
        }
        if (jobj != nullptr) {
            obj = getJNIEnv()->NewGlobalRef(jobj);
        } else {
            obj = nullptr;
        }
        method = jmth;
    }

    ~jLambda() {
        set(nullptr, nullptr);
    }

    operator bool() const {
        return obj != nullptr;
    }
};

void modalLoop();

void showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showOpenMultipleFiles(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showSaveFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showOpenFolder(GLFWwindow* window, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

#endif //FLAT_SYSTEM_BASE_H
