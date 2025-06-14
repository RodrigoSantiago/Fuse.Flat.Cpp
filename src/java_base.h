//
// Created by Rodrigo on 26/03/2025.
//

#ifndef FLAT_JAVA_BASE_H
#define FLAT_JAVA_BASE_H

#include <jni.h>
#include <iostream>

JNIEnv* getJNIEnv();
void releaseJNIEnv();
jstring newStringFromUTF8(JNIEnv* jEnv, const char* urf8);
std::string newUTF8FromString(JNIEnv* env, jstring javaString);
int convertIndex(const char* utf8, int byteOffset);

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

#endif //FLAT_JAVA_BASE_H
