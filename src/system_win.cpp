//
// Created by Rodrigo on 21/03/2025.
//

#include "system_base.h"
#include "nfd.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <thread>
#include <windows.h>
#include <vector>

void modalLoop() {
    NFD_Loop();
}

void showOpenFile(GLFWwindow* window, const char *sFileFilters, const char *sInitialFolder, jLambda<void(jlong,jstring)> result) {
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));
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
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));
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
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));
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
    void* nativeWindow = window == NULL ? NULL : reinterpret_cast<void*>(glfwGetWin32Window(window));
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

char* GetPixelsFromHBITMAP(HBITMAP hBitmap, BITMAPINFOHEADER& bmiHeader, long& imageSize) {
    if (!hBitmap) return nullptr;

    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);

    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = bm.bmWidth;
    bmiHeader.biHeight = bm.bmHeight;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 32;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0;
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 0;
    bmiHeader.biClrImportant = 0;

    long trueBytesPerRow = bm.bmWidth * (bmiHeader.biBitCount / 8);
    imageSize = trueBytesPerRow * abs(bm.bmHeight);

    char* pPixels = static_cast<char*>(GlobalAlloc(GMEM_FIXED, imageSize));
    if (!pPixels) return nullptr;

    HDC hdc = CreateCompatibleDC(NULL);
    if (!hdc) {
        GlobalFree(pPixels);
        return nullptr;
    }

    int scanlines = GetDIBits(hdc, hBitmap, 0, abs(bm.bmHeight), pPixels, (BITMAPINFO*)&bmiHeader, DIB_RGB_COLORS);

    DeleteDC(hdc);

    if (scanlines == 0) {
        GlobalFree(pPixels);
        return nullptr;
    }

    return pPixels;
}

int getClipboardImage(Image& imagem) {
    imagem.width = 0;
    imagem.height = 0;
    imagem.channels = 0;
    imagem.bytes.clear();

    if (!OpenClipboard(NULL)) {
        return -1;
    }

    HGLOBAL hData = NULL;
    UINT clipboardFormat = 0;

    if (IsClipboardFormatAvailable(CF_DIBV5)) {
        hData = GetClipboardData(CF_DIBV5);
        clipboardFormat = CF_DIBV5;
    } else if (IsClipboardFormatAvailable(CF_DIB)) {
        hData = GetClipboardData(CF_DIB);
        clipboardFormat = CF_DIB;
    } else if (IsClipboardFormatAvailable(CF_BITMAP)) {
        hData = GetClipboardData(CF_BITMAP);
        clipboardFormat = CF_BITMAP;
    }

    if (hData == NULL) {
        CloseClipboard();
        return -1;
    }

    BITMAPINFOHEADER bmiHeader;
    char* pRawPixels = NULL;
    HGLOBAL hDibMemory = NULL;

    if (clipboardFormat == CF_DIB || clipboardFormat == CF_DIBV5) {
        BITMAPINFO* pBmi = static_cast<BITMAPINFO*>(GlobalLock(hData));
        if (pBmi == NULL) {
            CloseClipboard();
            return -1;
        }

        bmiHeader = pBmi->bmiHeader;

        int colorTableSize = 0;
        if (pBmi->bmiHeader.biClrUsed > 0) {
            colorTableSize = pBmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);
        } else if (pBmi->bmiHeader.biBitCount <= 8) {
            colorTableSize = (1 << pBmi->bmiHeader.biBitCount) * sizeof(RGBQUAD);
        }

        pRawPixels = (char*)pBmi + pBmi->bmiHeader.biSize + colorTableSize;

    } else if (clipboardFormat == CF_BITMAP) {
        HBITMAP hBitmap = static_cast<HBITMAP>(hData);
        long extractedImageSize = 0;
        pRawPixels = GetPixelsFromHBITMAP(hBitmap, bmiHeader, extractedImageSize);
        if (pRawPixels == NULL) {
            GlobalUnlock(hData);
            CloseClipboard();
            return -1;
        }
        hDibMemory = GlobalHandle(pRawPixels);
    }

    imagem.width = bmiHeader.biWidth;
    imagem.height = abs(bmiHeader.biHeight);

    if (bmiHeader.biBitCount == 24) {
        imagem.channels = 3;
    } else if (bmiHeader.biBitCount == 32) {
        imagem.channels = 4;
    } else {
        if (clipboardFormat == CF_DIB || clipboardFormat == CF_DIBV5) GlobalUnlock(hData);
        if (hDibMemory) GlobalFree(hDibMemory);
        CloseClipboard();
        return -1;
    }

    long trueBytesPerRow = imagem.width * imagem.channels;
    imagem.bytes.resize(trueBytesPerRow * imagem.height);

    long paddedBytesPerRow = ((imagem.width * bmiHeader.biBitCount + 31) / 32) * 4;

    if (bmiHeader.biHeight > 0) {
        for (long y = 0; y < imagem.height; ++y) {
            char* srcRow = pRawPixels + (imagem.height - 1 - y) * paddedBytesPerRow;
            char* destRow = imagem.bytes.data() + y * trueBytesPerRow;
            std::copy(srcRow, srcRow + trueBytesPerRow, destRow);
        }
    } else {
        for (long y = 0; y < imagem.height; ++y) {
            char* srcRow = pRawPixels + y * paddedBytesPerRow;
            char* destRow = imagem.bytes.data() + y * trueBytesPerRow;
            std::copy(srcRow, srcRow + trueBytesPerRow, destRow);
        }
    }

    for (long y = 0; y < imagem.height; ++y) {
        for (long x = 0; x < imagem.width; ++x) {
            long p = (y * imagem.width + x) * imagem.channels;
            std::swap(imagem.bytes[p], imagem.bytes[p + 2]);  // BGR -> RGB
        }
    }

    if (clipboardFormat == CF_DIB || clipboardFormat == CF_DIBV5) {
        GlobalUnlock(hData);
    }
    if (hDibMemory) {
        GlobalFree(hDibMemory);
    }

    CloseClipboard();
    return 1;
}

void setClipboardImage(Image& imageData) {
    if (imageData.width <= 0 || imageData.height <= 0 || imageData.channels != 4 || imageData.bytes.empty()) {
        return;
    }

    const DWORD imageSize = imageData.width * imageData.height * imageData.channels;
    const size_t headerSize = sizeof(BITMAPV5HEADER);
    const size_t totalSize = headerSize + imageSize;

    HGLOBAL hDIB = GlobalAlloc(GMEM_MOVEABLE, totalSize);
    if (hDIB == NULL) return;

    void* pDIB = GlobalLock(hDIB);
    if (pDIB == NULL) {
        GlobalFree(hDIB);
        return;
    }

    BITMAPV5HEADER* bV5Hdr = static_cast<BITMAPV5HEADER*>(pDIB);
    ZeroMemory(bV5Hdr, sizeof(BITMAPV5HEADER));
    bV5Hdr->bV5Size = sizeof(BITMAPV5HEADER);
    bV5Hdr->bV5Width = imageData.width;
    bV5Hdr->bV5Height = -imageData.height;
    bV5Hdr->bV5Planes = 1;
    bV5Hdr->bV5BitCount = 32;
    bV5Hdr->bV5Compression = BI_BITFIELDS;
    bV5Hdr->bV5RedMask   = 0x00FF0000;
    bV5Hdr->bV5GreenMask = 0x0000FF00;
    bV5Hdr->bV5BlueMask  = 0x000000FF;
    bV5Hdr->bV5AlphaMask = 0xFF000000;

    byte* dibPixelData = reinterpret_cast<byte*>(bV5Hdr + 1);

    std::vector<char> convertedData = imageData.bytes;
    for (int y = 0; y < imageData.height; ++y) {
        for (int x = 0; x < imageData.width; ++x) {
            long pixelIndex = (long)(y * imageData.width + x) * 4;
            std::swap(convertedData[pixelIndex + 0], convertedData[pixelIndex + 2]); // R <-> B
        }
    }

    memcpy(dibPixelData, convertedData.data(), imageSize);
    GlobalUnlock(hDIB);

    if (!OpenClipboard(NULL)) {
        GlobalFree(hDIB);
        return;
    }

    EmptyClipboard();

    if (SetClipboardData(CF_DIBV5, hDIB) == NULL) {
        GlobalFree(hDIB);
        CloseClipboard();
        return;
    }

    CloseClipboard();
}