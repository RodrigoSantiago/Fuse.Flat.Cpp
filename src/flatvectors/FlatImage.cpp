//
// Created by Rodrigo on 16/03/2025.
//

#include "FlatImage.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
    int last_pos;
    int size;
    void *context;
} custom_stbi_mem_context;

static void custom_stbi_write_mem(void *context, void *data, int size) {
    custom_stbi_mem_context *c = (custom_stbi_mem_context*)context;

    int cur_pos = c->last_pos;
    if (cur_pos + size > c->size) {
        while (c->size < cur_pos + size) {
            c->size = c->size * 2;
        }
        c->context = realloc(c->context, sizeof(unsigned char) * c->size);
    }

    char *dst = (char *)c->context;
    char *src = (char *)data;

    for (int i = 0; i < size; i++) {
        dst[cur_pos++] = src[i];
    }
    c->last_pos = cur_pos;
}

unsigned char* FlatImage::writePng(unsigned char* data, int width, int height, int channels, int* dataSize) {
    return stbi_write_png_to_mem(data, 0, width, height, channels, dataSize);
}

void FlatImage::freePng(unsigned char* data) {
    STBIW_FREE(data);
}

unsigned char* FlatImage::writeJpg(unsigned char* data, int width, int height, int channels, int quality, int* dataSize) {
    custom_stbi_mem_context context;
    context.last_pos = 0;
    context.size = 256;
    context.context = malloc(sizeof(unsigned char) * 256);
    if (!stbi_write_jpg_to_func(custom_stbi_write_mem, &context, width, height, channels, data, quality)) {
        free(context.context);
        return NULL;
    }
    *dataSize = context.last_pos;
    return (unsigned char *) context.context;
}

void FlatImage::freeJpg(unsigned char* data) {
    free(data);
}

unsigned char* FlatImage::writeBmp(unsigned char* data, int width, int height, int channels, int* dataSize) {
    custom_stbi_mem_context context;
    context.last_pos = 0;
    context.size = 256;
    context.context = malloc(sizeof(unsigned char) * 256);
    if (!stbi_write_bmp_to_func(custom_stbi_write_mem, &context, width, height, channels, data)) {
        free(context.context);
        return NULL;
    }
    *dataSize = context.last_pos;
    return (unsigned char *) context.context;
}

void FlatImage::freeBmp(unsigned char* data) {
    free(data);
}

unsigned char* FlatImage::writeTga(unsigned char* data, int width, int height, int channels, int* dataSize) {
    custom_stbi_mem_context context;
    context.last_pos = 0;
    context.size = 256;
    context.context = malloc(sizeof(unsigned char) * 256);
    if (!stbi_write_tga_to_func(custom_stbi_write_mem, &context, width, height, channels, data)) {
        free(context.context);
        return NULL;
    }
    *dataSize = context.last_pos;
    return (unsigned char *) context.context;
}

void FlatImage::freeTga(unsigned char* data) {
    free(data);
}