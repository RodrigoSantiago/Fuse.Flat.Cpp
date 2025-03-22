//
// Created by Rodrigo on 21/03/2025.
//

#ifndef FLAT_SYSTEM_BASE_H
#define FLAT_SYSTEM_BASE_H

#include <iostream>
#include <GLFW/glfw3.h>

std::string showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder);

std::string showOpenMultipleFiles(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder);

std::string showSaveFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder);

std::string showOpenFolder(GLFWwindow* window, const char *sInitialFolder);

#endif //FLAT_SYSTEM_BASE_H
