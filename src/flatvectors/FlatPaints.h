//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATPAINTS_H
#define FLAT_FLATPAINTS_H

#include "FlatVectorsBase.h"

class FlatPaints {
public:
    static void setColorPaint(fvUniform& uniform, int32 color);
    static void setImagePaint(fvUniform& uniform, float* affineImg, int32 color, int32 cycleMethod, int32 nearest);
    static void setLinearGradientPaint(fvUniform& uniform, float* affine,
                                       float x1, float y1, float x2, float y2,
                                       int32 count, float* stops, int32* colors, int32 cycleMethod);
    static void setRadialGradientPaint(fvUniform& uniform, float* affine,
                                       float x, float y, float rIn, float rOut, float fx, float fy,
                                       int32 count, float* stops, int32* colors, int32 cycleMethod);
    static void setBoxGradientPaint(fvUniform& uniform, float* affine,
                                    float x, float y, float w, float h, float r, float f, float a, int32 c);
};


#endif //FLAT_FLATPAINTS_H
