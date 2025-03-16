//
// Created by Rodrigo on 13/03/2025.
//

#ifndef FLAT_FLATVECTORSBASE_H
#define FLAT_FLATVECTORSBASE_H

#include <vector>

class FlatVectors;
class FlatRender;
class FlatFont;
class FlatFontRender;
class FlatPack;
class FlatPaints;
class FlatText;
class FlatCurves;
class FlatImage;

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

typedef struct fvPoint {
    float x;
    float y;
} fvPoint;

typedef struct fvTriangle {
    int a;
    int b;
    int c;
} fvTriangle;

typedef struct fvUniform {
    // Buffer
    float type;                 // [0] - Color|Grad, [1] - Color + Image, [2] - Text + Color, [3] - Text + Color + Image
    float joinType;             // Round / Bevel
    float cycleType;            // Clamp / Cycle / Reflect
    float stopCount;            // 0 -- 16
    float colorMat[12];
    float imageMat[12];
    float shape[4];             // Extent[0,1], Radius [2], Feather [3]
    float extra[4];             // Focus[0,1], Circle/Rect/Shadow/StrokeShadow[2], Blur [3]
    float stops[16];
    float colors[64];
} fvUniform;

typedef struct fvPaint {
    fvPathOp pathOp;
    fvWindingRule pathRule;
    int elements;
    int vertices;
    FlatFontRender* font;
    unsigned long int image0;
    int antiAlias;
    int convex;
    float transform[12];
} fvPaint;

typedef struct fvGlyph {
    int enabled;
    float advance;
    long int unicode;
    unsigned char* cell;

    float x;
    float y;
    float w;
    float h;
} fvGlyph;

#endif //FLAT_FLATVECTORSBASE_H
