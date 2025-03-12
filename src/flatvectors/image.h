//
// Created by Rodrigo on 12/03/2025.
//

#ifndef FLAT_IMAGE_H
#define FLAT_IMAGE_H

unsigned char* writePng(unsigned char* data, int width, int height, int channels, int* dataSize);

void freePng(unsigned char* data);

unsigned char* writeJpg(unsigned char* data, int width, int height, int channels, int quality, int* dataSize);

void freeJpg(unsigned char* data);

unsigned char* writeBmp(unsigned char* data, int width, int height, int channels, int* dataSize);

void freeBmp(unsigned char* data);

unsigned char* writeTga(unsigned char* data, int width, int height, int channels, int* dataSize);

void freeTga(unsigned char* data);

#endif //FLAT_IMAGE_H
