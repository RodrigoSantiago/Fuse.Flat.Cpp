//
// Created by Rodrigo on 21/03/2025.
//

#ifndef FLAT_SYSTEM_BASE_H
#define FLAT_SYSTEM_BASE_H

#include "java_base.h"
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

struct Image {
    long width;
    long height;
    long channels;
    std::vector<char> bytes;
};

void modalLoop();

void showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showOpenMultipleFiles(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showSaveFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

void showOpenFolder(GLFWwindow* window, const char *sInitialFolder, jLambda<void(jlong, jstring)> result);

int getClipboardImage(Image& imagem);

void setClipboardImage(Image& imagem);

#endif //FLAT_SYSTEM_BASE_H
