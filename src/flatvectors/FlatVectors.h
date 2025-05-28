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
    static FlatEmoji* emoji;
    static bool emojiRender;

    FlatRender* render;

    fvPaint paint;
    fvUniform uniform;

    std::vector<fvPaint> draws;
    std::vector<char> uniforms;
    std::vector<fvTriangle> elm;
    std::vector<fvPoint> vtx;
    std::vector<fvPoint> uvs;

    fvPathStep lastStep;
    int32 curVtxIndex;            // INDEX
    int32 curElmIndex;            // INDEX
    int32 curDrwIndex;            // INDEX
    int32 curDrawBeginVtxIndex;   // INDEX
    int32 curDrawBeginElmIndex;   // INDEX
    int32 curShapeBeginVtxIndex;  // INDEX
    int32 curShapeBeginElmIndex;  // INDEX
    float curPosX;
    float curPosY;
    float strokeStartPosX;
    float strokeStartPosY;
    int32 strokeFirstLineVtxIndex;
    float strokeFirstLinePosX;
    float strokeFirstLinePosY;
    int32 prevVtxIndex;
    float prevLineStartPosX;
    float prevLineStartPosY;
    int32 curveJoin;

    float fontScale;
    float fontSpacing;
    float fontBlur;

    FlatStroke stroke;
    float dashAmount;
    int32 dashPhase;
    int32 dashFill;
    float scale;
public:
    FlatVectors();
    ~FlatVectors();
private:
    bool ensureSpace(int32 vertex, int32 elements);
    int32 addVertex(float x, float y, float u, float v);
    int32 addTriangle(int32 elA, int32 elB, int32 elC);
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
    void strokeJoin(int32 vx0, int32 vx1, float x1, float y1, float x2, float y2, float x3, float y3, bool cruve);
    void strokeClose();

    void strokeBevel(float det, int32 v0, int32 v1);
    void strokeRound(fvPoint from, fvPoint to, fvPoint center);
    void strokeDashTo(float x, float y, bool curve);

    void shapeDiscard();
    void shapeDrawDiscard();

    void pushToRender();
public:
    static void setDebug(bool debug);
    static void setEmojiRender(bool enabled);
    static bool isEmojiRender();
    static FlatEmoji* getEmoji();
    static void setEmoji(FlatEmoji* emoji);

    FlatRender* getRender();

    void beginFrame(int32 width, int32 height);
    void endFrame();
    void flush();

    void setColor(fvUniform& color, int32 img);
    void setStroke(FlatStroke& stroke);
    void setTransform(float m00, float m10, float m01, float m11, float m02, float m12);
    void setAntiAliasing(int32 enabled);
    void setFont(FlatFont* font);
    void setFontScale(float fscale);
    void setFontSpacing(float fspacing);
    void setFontBlur(float fblur);
    void setBlendMode(fvBlendMode blendMode);

    void begin(fvPathOp pathOp, fvWindingRule pathRule);
    void end();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void quadTo(float cx, float cy, float x, float y);
    void cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
    void close();

    void rect(float x, float y, float width, float height, bool fill);
    void ellipse(float x, float y, float width, float height, bool fill);
    void roundRect(float x, float y, float width, float height, float c1, float c2, float c3, float c4, bool fill);

    void clearClip(bool clip);

    void text(const char* str, int32 strLen, float x, float y, float maxWidth, float maxHeight);
};


#endif //FLAT_FLATVECTORS_H
