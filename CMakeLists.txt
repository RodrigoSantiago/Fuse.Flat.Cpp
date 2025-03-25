cmake_minimum_required(VERSION 3.6)
project(Flat)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)
set(CMAKE_CXX_STANDARD 11)

include_directories(include
        src/gl
        src/ml
        src/wl
        src/svg
        src/flatvectors
        ${JNI_INCLUDE_DIRS})

if(WIN32)
    link_directories(lib/win)
    add_definitions(-DWINDOWS_PLATFORM)
    set(SOURCE_FILES
            src/nfd/nfd_common.cpp
            src/nfd/nfd_win.cpp
            src/system_win.cpp
            )
elseif(UNIX)
    link_directories(lib/unix)
    add_definitions(-DUNIX_PLATFORM)
    set(SOURCE_FILES
            src/nfd/nfd_common.cpp
            src/nfd/nfd_gtk.cpp
            src/system_unix.cpp)
endif()

list(APPEND SOURCE_FILES
        src/glad.c
        src/wl/flat_backend_WL.cpp
        src/gl/flat_backend_GL.cpp
        src/ml/flat_backend_ML.cpp
        src/svg/flat_backend_SVG.cpp

        src/flatvectors/FlatVectors.cpp
        src/flatvectors/FlatRender.cpp
        src/flatvectors/FlatFont.cpp
        src/flatvectors/FlatPack.cpp
        src/flatvectors/FlatFontRender.cpp
        src/flatvectors/FlatPaints.cpp
        src/flatvectors/FlatText.cpp
        src/flatvectors/FlatCurves.cpp
        src/flatvectors/FlatImage.cpp
        src/flatvectors/FlatStroke.cpp
        src/flatvectors/FlatEmoji.cpp)

add_library(Flat SHARED ${SOURCE_FILES})
#add_executable(Flat ${SOURCE_FILES} main.cpp)

if(WIN32)
    target_link_libraries(Flat opengl32 gdi32 glfw3.a -static-libstdc++)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    target_link_libraries(Flat glfw3 GL X11 pthread dl ${GTK3_LIBRARIES})
endif()