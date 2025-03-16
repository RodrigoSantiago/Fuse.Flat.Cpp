//
// Created by Rodrigo on 16/03/2025.
//

#ifndef FLAT_FLATIMAGE_H
#define FLAT_FLATIMAGE_H

#include "FlatVectorsBase.h"

class FlatImage {
public:
    static unsigned char* writePng(unsigned char* data, int width, int height, int channels, int* dataSize);

    static void freePng(unsigned char* data);

    static unsigned char* writeJpg(unsigned char* data, int width, int height, int channels, int quality, int* dataSize);

    static void freeJpg(unsigned char* data);

    static unsigned char* writeBmp(unsigned char* data, int width, int height, int channels, int* dataSize);

    static void freeBmp(unsigned char* data);

    static unsigned char* writeTga(unsigned char* data, int width, int height, int channels, int* dataSize);

    static void freeTga(unsigned char* data);
};


#endif //FLAT_FLATIMAGE_H
