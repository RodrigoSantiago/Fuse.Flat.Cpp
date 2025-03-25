//
// Created by Rodrigo on 21/03/2025.
//

#include "system_base.h"
#include "nfd.h"

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <thread>

void modalLoop() {
    NFD_Loop();
}

void showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong,jstring)> result) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetX11Window(window));
    jlong windowId = (jlong)window;
    std::string fileFilters = sFileFilters;
    std::string initialFolder = sInitialFolder;

    std::thread localThread([=]() mutable {
        std::string nfdResult = NFD_OpenDialog(fileFilters.c_str(), initialFolder.c_str(), nativeWindow);
        if (!nfdResult.empty()) {
            result.run(windowId, newStringFromUTF8(getJNIEnv(), nfdResult.c_str()));
        } else {
            result.run(windowId, nullptr);
        }
        result.set(nullptr, nullptr);
        releaseJNIEnv();
    });
    localThread.detach();
}

void showOpenMultipleFiles(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong,jstring)> result) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetX11Window(window));
    jlong windowId = (jlong)window;
    std::string fileFilters = sFileFilters;
    std::string initialFolder = sInitialFolder;

    std::thread localThread([result, windowId, nativeWindow, fileFilters, initialFolder]() mutable {
        std::string nfdResult = NFD_OpenDialogMultiple(fileFilters.c_str(), initialFolder.c_str(), nativeWindow);
        if (!nfdResult.empty()) {
            result.run(windowId, newStringFromUTF8(getJNIEnv(), nfdResult.c_str()));
        } else {
            result.run(windowId, nullptr);
        }
        result.set(nullptr, nullptr);
        releaseJNIEnv();
    });
    localThread.detach();

}

void showSaveFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong,jstring)> result) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetX11Window(window));
    jlong windowId = (jlong)window;
    std::string fileFilters = sFileFilters;
    std::string initialFolder = sInitialFolder;

    std::thread localThread([result, windowId, nativeWindow, fileFilters, initialFolder]() mutable {
        std::string nfdResult = NFD_SaveDialog(fileFilters.c_str(), initialFolder.c_str(), nativeWindow);
        if (!nfdResult.empty()) {
            result.run(windowId, newStringFromUTF8(getJNIEnv(), nfdResult.c_str()));
        } else {
            result.run(windowId, nullptr);
        }
        result.set(nullptr, nullptr);
        releaseJNIEnv();
    });
    localThread.detach();
}

void showOpenFolder(GLFWwindow* window, const char *sInitialFolder, jLambda<void(jlong,jstring)> result) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetX11Window(window));
    jlong windowId = (jlong)window;
    std::string initialFolder = sInitialFolder;

    std::thread localThread([result, windowId, nativeWindow, initialFolder]() mutable {
        std::string nfdResult = NFD_PickFolder(initialFolder.c_str(), nativeWindow);
        if (!nfdResult.empty()) {
            result.run(windowId, newStringFromUTF8(getJNIEnv(), nfdResult.c_str()));
        } else {
            result.run(windowId, nullptr);
        }
        result.set(nullptr, nullptr);
        releaseJNIEnv();
    });
    localThread.detach();
}