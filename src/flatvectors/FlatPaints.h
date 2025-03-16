//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATPAINTS_H
#define FLAT_FLATPAINTS_H

#include "FlatVectorsBase.h"

class FlatPaints {
public:
    static void setColorPaint(fvUniform& uniform, long color);
    static void setImagePaint(fvUniform& uniform, float* affineImg, long color, int cycleMethod);
    static void setLinearGradientPaint(fvUniform& uniform, float* affine,
                                       float x1, float y1, float x2, float y2,
                                       int count, float* stops, long* colors, int cycleMethod);
    static void setRadialGradientPaint(fvUniform& uniform, float* affine,
                                       float x, float y, float rIn, float rOut, float fx, float fy,
                                       int count, float* stops, long* colors, int cycleMethod);
    static void setBoxGradientPaint(fvUniform& uniform, float* affine,
                                    float x, float y, float w, float h, float r, float f, float a, long c);
};


#endif //FLAT_FLATPAINTS_H
