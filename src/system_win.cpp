//
// Created by Rodrigo on 21/03/2025.
//

#include "system_base.h"
#include "nfd.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

std::string showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));

    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(sFileFilters, sInitialFolder, &outPath, nativeWindow);

    if (result == NFD_OKAY) {
        std::string str = outPath;
        free(outPath);
        return str;
    }
    return "";
}

std::string showOpenMultipleFiles(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));

    nfdpathset_t outPath = {};
    nfdresult_t result = NFD_OpenDialogMultiple(sFileFilters, sInitialFolder, &outPath, nativeWindow);

    if (result == NFD_OKAY) {
        std::string str;
        int count = NFD_PathSet_GetCount(&outPath);
        for (int i = 0; i < count; ++i) {
            if (i > 0)
                str += ",";
            str += NFD_PathSet_GetPath(&outPath, i);
        }
        NFD_PathSet_Free(&outPath);
        return str;
    }
    return "";
}

std::string showSaveFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));

    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_SaveDialog(sFileFilters, sInitialFolder, &outPath, nativeWindow);

    if (result == NFD_OKAY) {
        std::string str = outPath;
        free(outPath);
        return str;
    }
    return "";
}

std::string showOpenFolder(GLFWwindow* window, const char *sInitialFolder) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));

    nfdchar_t *outPath = NULL;
    nfdresult_t result = NFD_PickFolder(sInitialFolder, &outPath, nativeWindow);

    if (result == NFD_OKAY) {
        std::string str = outPath;
        free(outPath);
        return str;
    }
    return "";
}