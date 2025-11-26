//
// Created by Rodrigo on 15/03/2025.
//

#include <cmath>
#include "FlatPaints.h"

void identity(float* t) {
    t[0] = 1.0f;
    t[1] = 0.0f;
    t[2] = 0.0f;
    t[3] = 1.0f;
    t[4] = 0.0f;
    t[5] = 0.0f;
}

void multiply(float* t, const float* s) {
    float t0 = t[0] * s[0] + t[1] * s[2];
    float t2 = t[2] * s[0] + t[3] * s[2];
    float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
    t[1] = t[0] * s[1] + t[1] * s[3];
    t[3] = t[2] * s[1] + t[3] * s[3];
    t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
    t[0] = t0;
    t[2] = t2;
    t[4] = t4;
}

void FlatPaints::setColorPaint(fvUniform& uniform, int32 color) {
    uniform = {};
    uniform.type = 0;

    identity(uniform.colorMat);

    uniform.shape[0] = 0;
    uniform.shape[1] = 0;
    uniform.shape[2] = 0;
    uniform.shape[3] = 0;

    uniform.stopCount = 0;
    uniform.filter = 0;
    uniform.colors[0] = ((color >> 24) & 0xFF) / 255.f;
    uniform.colors[1] = ((color >> 16) & 0xFF) / 255.f;
    uniform.colors[2] = ((color >> 8) & 0xFF) / 255.f;
    uniform.colors[3] = ((color >> 0) & 0xFF) / 255.f;
    uniform.cycleType = 0;
}

void FlatPaints::setImagePaint(fvUniform& uniform, float* affineImg, int32 color, int32 cycleMethod, int32 nearest) {
    uniform = {};
    uniform.type = 3;
    if (affineImg != 0) {
        for (int32 i = 0; i < 6; i++) {
            uniform.colorMat[i] = affineImg[i];
        }
    } else {
        identity(uniform.colorMat);
    }

    uniform.shape[0] = 0;
    uniform.shape[1] = 0;
    uniform.shape[2] = 0;
    uniform.shape[3] = 0;

    uniform.stopCount = 0;
    uniform.filter = nearest ? 1 : 0;
    uniform.colors[0] = ((color >> 24) & 0xFF) / 255.f;
    uniform.colors[1] = ((color >> 16) & 0xFF) / 255.f;
    uniform.colors[2] = ((color >> 8) & 0xFF) / 255.f;
    uniform.colors[3] = ((color >> 0) & 0xFF) / 255.f;
    uniform.cycleType = cycleMethod;
}

void FlatPaints::setLinearGradientPaint(fvUniform& uniform, float* affine,
                                        float x1, float y1, float x2, float y2,
                                        int32 count, float* stops, int32* colors, int32 cycleMethod) {
    uniform = {};
    uniform.type = count == 0 ? 0 : 1;

    float dx, dy, d;
    const float large = 1e5;

    dx = x2 - x1;
    dy = y2 - y1;
    d = sqrtf(dx * dx + dy * dy);
    if (d > 0.0001f) {
        dx /= d;
        dy /= d;
    } else {
        dx = 0;
        dy = 1;
    }

    uniform.colorMat[0] = dy;
    uniform.colorMat[1] = -dx;
    uniform.colorMat[2] = dx;
    uniform.colorMat[3] = dy;
    uniform.colorMat[4] = x1 - dx * large;
    uniform.colorMat[5] = y1 - dy * large;
    multiply(uniform.colorMat, affine);

    uniform.shape[0] = large;
    uniform.shape[1] = large + d * 0.5f;
    uniform.shape[2] = 0.0f;
    uniform.shape[3] = d < 1.0f ? 1.0f : d;

    uniform.stopCount = count - 1;
    uniform.filter = 0;
    for (int32 i = 0; i < count; i++) {
        uniform.stops[i] = stops[i];
        uniform.colors[i * 4] = ((colors[i] >> 24) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 1] = ((colors[i] >> 16) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 2] = ((colors[i] >> 8) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 3] = ((colors[i] >> 0) & 0xFF) / 255.f;
    }
    for (int32 i = count; i < 16; i++) {
        uniform.stops[i] = 2.0;
    }
    if (count > 0) {
        for (int32 i = count; i < 16; i++) {
            uniform.colors[i * 4] = ((colors[count - 1] >> 24) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 1] = ((colors[count - 1] >> 16) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 2] = ((colors[count - 1] >> 8) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 3] = ((colors[count - 1] >> 0) & 0xFF) / 255.f;
        }
    }
    uniform.cycleType = cycleMethod;
}

void FlatPaints::setRadialGradientPaint(fvUniform& uniform, float* affine,
                                        float x, float y, float rIn, float rOut, float fx, float fy,
                                        int32 count, float* stops, int32* colors, int32 cycleMethod) {
    uniform = {};
    uniform.type = count == 0 ? 0 : 1;

    float r = (rIn+rOut)*0.5f;
    float f = (rOut-rIn);

    uniform.colorMat[0] = 1.0f;
    uniform.colorMat[1] = 0.0f;
    uniform.colorMat[2] = 0.0f;
    uniform.colorMat[3] = 1.0f;
    uniform.colorMat[4] = x;
    uniform.colorMat[5] = y;
    multiply(uniform.colorMat, affine);

    uniform.shape[0] = r;
    uniform.shape[1] = r;
    uniform.shape[2] = r;
    uniform.shape[3] = f < 1.0f ? 1.0f : f;

    uniform.stopCount = count - 1;
    uniform.filter = 0;
    for (int32 i = 0; i < count; i++) {
        uniform.stops[i] = stops[i];
        uniform.colors[i * 4] = ((colors[i] >> 24) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 1] = ((colors[i] >> 16) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 2] = ((colors[i] >> 8) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 3] = ((colors[i] >> 0) & 0xFF) / 255.f;
    }
    for (int32 i = count; i < 16; i++) {
        uniform.stops[i] = 2.0;
    }
    if (count > 0) {
        for (int32 i = count; i < 16; i++) {
            uniform.colors[i * 4] = ((colors[count - 1] >> 24) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 1] = ((colors[count - 1] >> 16) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 2] = ((colors[count - 1] >> 8) & 0xFF) / 255.f;
            uniform.colors[i * 4 + 3] = ((colors[count - 1] >> 0) & 0xFF) / 255.f;
        }
    }
    uniform.cycleType = cycleMethod;

    uniform.extra[0] = (fx = rOut < 0.0001f ? 0 : (fx - x) / rOut);
    uniform.extra[1] = (fy = rOut < 0.0001f ? 0 : (fy - y) / rOut);
    if (fx < -0.0001 || fx > 0.0001 || fy < -0.0001 || fy > 0.0001) {
        uniform.extra[2] = 1;
    }
}

void FlatPaints::setBoxGradientPaint(fvUniform& uniform, float* affine,
                                     float x, float y, float w, float h, float r, float f, float a, int32 c) {
    uniform = {};
    uniform.type = 2;

    uniform.colorMat[0] = 1.0f;
    uniform.colorMat[1] = 0.0f;
    uniform.colorMat[2] = 0.0f;
    uniform.colorMat[3] = 1.0f;
    uniform.colorMat[4] = x + w * 0.5f;
    uniform.colorMat[5] = y + h * 0.5f;
    if (affine != NULL) {
        multiply(uniform.colorMat, affine);
    }

    uniform.shape[0] = w * 0.5f;
    uniform.shape[1] = h * 0.5f;
    uniform.shape[2] = r;
    uniform.shape[3] = f < 1.0f ? 1.0f : f;

    uniform.stopCount = 1;
    uniform.filter = 0;

    uniform.stops[0] = 0;
    uniform.colors[0] = ((c >> 24) & 0xFF) / 255.f;
    uniform.colors[1] = ((c >> 16) & 0xFF) / 255.f;
    uniform.colors[2] = ((c >> 8) & 0xFF) / 255.f;
    uniform.colors[3] = ((c >> 0) & 0xFF) / 255.f * a;

    uniform.stops[1] = 1;
    uniform.colors[4] = ((c >> 24) & 0xFF) / 255.f;
    uniform.colors[5] = ((c >> 16) & 0xFF) / 255.f;
    uniform.colors[6] = ((c >> 8) & 0xFF) / 255.f;
    uniform.colors[7] = 0;
    for (int32 i = 2; i < 16; i++) {
        uniform.stops[i] = 2.0;
    }
    for (int32 i = 2; i < 16; i++) {
        uniform.colors[i * 4] = ((c >> 24) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 1] = ((c >> 16) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 2] = ((c >> 8) & 0xFF) / 255.f;
        uniform.colors[i * 4 + 3] = 0;
    }
    uniform.cycleType = 4;
}