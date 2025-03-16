//
// Created by Rodrigo on 13/03/2025.
//

#ifndef FLAT_FLATVECTORS_H
#define FLAT_FLATVECTORS_H

#include "FlatVectorsBase.h"
#include "FlatStroke.h"
#include <vector>

class FlatVectors {
    static bool debug;
    static float* icos;
    static float* isin;

    FlatRender* render;

    fvPaint paint{};
    fvUniform uniform{};

    std::vector<fvPaint> draws;
    std::vector<char> uniforms;
    std::vector<fvTriangle> elm;
    std::vector<fvPoint> vtx;
    std::vector<fvPoint> uvs;

    fvPathStep lastStep;
    int curVtxIndex;            // INDEX
    int curElmIndex;            // INDEX
    int curDrwIndex;            // INDEX
    int curDrawBeginVtxIndex;   // INDEX
    int curDrawBeginElmIndex;   // INDEX
    int curShapeBeginVtxIndex;  // INDEX
    int curShapeBeginElmIndex;  // INDEX
    float curPosX;
    float curPosY;
    float strokeStartPosX;
    float strokeStartPosY;
    int strokeFirstLineVtxIndex;
    float strokeFirstLinePosX;
    float strokeFirstLinePosY;
    int prevVtxIndex;
    float prevLineStartPosX;
    float prevLineStartPosY;
    int curveJoin;

    float fontScale;
    float fontSpacing;
    float fontBlur;

    FlatStroke stroke;
    float dashAmount;
    int dashPhase;
    int dashFill;
    float scale;
public:
    FlatVectors();
    ~FlatVectors();
private:
    void ensureSpace(int vertex, int elements);
    int addVertex(float x, float y, float u, float v);
    int addTriangle(int elA, int elB, int elC);
    bool equals(float x, float y);
    inline static float fastCos(float a);
    inline static float fastSin(float a);

    void fillBegin();
    void fillEnd();
    void fillMoveTo(float x, float y);
    void fillLineTo(float x, float y);
    static void fillCurveTo(FlatVectors* context, double x, double y);
    void fillClose();

    void strokeBegin();
    void strokeEnd();
    void strokeMoveTo(float x, float y);
    void strokeLineTo(float x, float y, bool curve);
    static void strokeCurveTo(FlatVectors* context, double x, double y);
    void strokeCap();
    void strokeJoin(int vx0, int vx1, float x1, float y1, float x2, float y2, float x3, float y3, bool cruve);
    void strokeClose();

    void strokeBevel(float det, int v0, int v1);
    void strokeRound(fvPoint from, fvPoint to, fvPoint center);
    void strokeDashTo(float x, float y, bool curve);

    void shapeDiscard();
    void shapeDrawDiscard();

    void pushToRender();
public:
    static void setDebug(bool debug);

    FlatRender* getRender();

    void beginFrame(int width, int height);
    void endFrame();
    void flush();

    void setColor(fvUniform& color, int img);
    void setStroke(FlatStroke& stroke);
    void setTransform(float m00, float m10, float m01, float m11, float m02, float m12);
    void setAntiAliasing(int enabled);
    void setFont(FlatFontRender* fontRender);
    void setFontScale(float fscale);
    void setFontSpacing(float fspacing);
    void setFontBlur(float fblur);

    void begin(fvPathOp pathOp, fvWindingRule pathRule);
    void end();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void quadTo(float cx, float cy, float x, float y);
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
    void close();

    void rect(float x, float y, float width, float height);
    void ellipse(float x, float y, float width, float height);
    void roundRect(float x, float y, float width, float height, float c1, float c2, float c3, float c4);

    void clearClip(bool clip);

    void text(const char* str, int strLen, float x, float y, float maxWidth, float maxHeight);
};


#endif //FLAT_FLATVECTORS_H
