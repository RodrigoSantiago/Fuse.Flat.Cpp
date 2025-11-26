//
// Created by Rodrigo on 13/03/2025.
//

#ifndef FLAT_FLATVECTORSBASE_H
#define FLAT_FLATVECTORSBASE_H

#include <vector>
#include <iostream>
#include "../flat_base_types.h"

class FlatVectors;
class FlatRender;
class FlatFont;
class FlatFontRender;
class FlatPack;
class FlatPaints;
class FlatText;
class FlatCurves;
class FlatImage;
class FlatEmoji;

enum fvPathStep {
    MOVE, LINE, CLOSE
};

enum fvPathOp {
    CONVEX, FILL, STROKE, CLIP, UNCLIP, TEXT
};

enum fvCap {
    CAP_BUTT, CAP_ROUND, CAP_SQUARE
};

enum fvJoin {
    JOIN_MITER, JOIN_ROUND, JOIN_BEVEL      //TODO - , ARCS, CLIP
};

enum fvWindingRule {
    EVEN_ODD, NON_ZERO
};

enum fvBlendMode {
    CLEAR,
    SRC,
    DST,
    SRC_OVER,
    DST_OVER,
    SRC_IN,
    DST_IN,
    SRC_OUT,
    DST_OUT,
    SRC_ATOP,
    DST_ATOP,
    XOR,
    ADD, SUB, MUL, LIGHTEN, DARKEN, REV_SUB
};

typedef struct fvPoint {
    float x;
    float y;
} fvPoint;

typedef struct fvTriangle {
    int32 a;
    int32 b;
    int32 c;
} fvTriangle;

typedef struct fvUniform {
    // Buffer
    float type;                 // [0] - Color, [1] - Grad, [2] - BoxGrad, [3] - Image, [4] - Text
    float filter;               // [0] - Linear/Default, [1] - Nearest
    float cycleType;            // Clamp / Cycle / Reflect / Empty
    float stopCount;            // 0 -- 16
    float colorMat[12];
    float shape[4];             // Extent[0,1], Radius [2], Feather [3]
    float extra[4];             // Focus[0,1], Circle/Rect/Shadow/StrokeShadow[2], Blur [3]
    float stops[16];
    float colors[64];
} fvUniform;

typedef struct fvPaint {
    fvPathOp pathOp;
    fvWindingRule pathRule;
    fvBlendMode blendMode;
    int32 elements;
    int32 vertices;
    FlatFont* font;
    uint32 image0;
    int32 antiAlias;
    int32 convex;
    int32 emojis;
    float transform[12];
} fvPaint;

typedef struct fvGlyph {
    int32 enabled;
    float advance;
    int32 unicode;
    unsigned char* cell;

    float x;
    float y;
    float w;
    float h;
} fvGlyph;

#endif //FLAT_FLATVECTORSBASE_H
