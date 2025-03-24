//
// Created by Rodrigo on 13/03/2025.
//

#include <iostream>
#include <cmath>
#include <cstring>
#include "FlatVectors.h"
#include "FlatRender.h"
#include "FlatFont.h"
#include "FlatFontRender.h"
#include "FlatText.h"
#include "FlatCurves.h"

// Local Private

#define PI 3.14159265359f
#define PI2 6.28318530718f

void affineToMat3(const float* affine, float* m3) {
    m3[8] = affine[4];
    m3[9] = affine[5];

    m3[4] = affine[2];
    m3[5] = affine[3];

    m3[0] = affine[0];
    m3[1] = affine[1];

    m3[2] = 0.0f;
    m3[3] = 0.0f;
    m3[6] = 0.0f;
    m3[7] = 0.0f;
    m3[10] = 1.0f;
    m3[11] = 0.0f;
}

void inverseMat(float* inv, const float* t) {
    double invdet, det = (double) t[0] * t[3] - (double) t[2] * t[1];
    invdet = 1.0 / det;
    float inv0 = (float) (t[3] * invdet);
    float inv2 = (float) (-t[2] * invdet);
    float inv4 = (float) (((double) t[2] * t[5] - (double) t[3] * t[4]) * invdet);
    float inv1 = (float) (-t[1] * invdet);
    float inv3 = (float) (t[0] * invdet);
    float inv5 = (float) (((double) t[1] * t[4] - (double) t[0] * t[5]) * invdet);
    inv[0] = inv0;
    inv[1] = inv1;
    inv[2] = inv2;
    inv[3] = inv3;
    inv[4] = inv4;
    inv[5] = inv5;
}

void expandline(float w,
                float x1, float y1, float x2, float y2,
                float& px1, float& py1, float& px2, float& py2,
                float& sx1, float& sy1, float& sx2, float& sy2) {
    w /= 2;
    float l = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    float y = w * (y2 - y1) / l;
    float x = w * (x1 - x2) / l;

    px1 = x1 + y;
    sx1 = x2 + y;
    py1 = y1 + x;
    sy1 = y2 + x;

    px2 = x1 - y;
    sx2 = x2 - y;
    py2 = y1 - x;
    sy2 = y2 - x;
}

float miterPoint(fvPoint p1, fvPoint p2, fvPoint p3, fvPoint p4, fvPoint& point) {
    float dx1 = p2.x - p1.x;
    float dy1 = p2.y - p1.y;
    float dx2 = p4.x - p3.x;
    float dy2 = p4.y - p3.y;

    float det = dx1 * dy2 - dy1 * dx2;

    if (std::abs(det) < 1e-6) {
        return 0;
    }

    float dx3 = p3.x - p1.x;
    float dy3 = p3.y - p1.y;

    float t1 = (dx3 * dy2 - dy3 * dx2) / det;

    float tx = p1.x + t1 * dx1;
    float ty = p1.y + t1 * dy1;

    point = {tx, ty};
    return t1;
}

float maxscale(const float* t) {
    float sx2 = std::abs(t[0] * t[0] + t[1] * t[1]);
    float sy2 = std::abs(t[2] * t[2] + t[3] * t[3]);
    if (sx2 > sy2) {
        return std::sqrt(sx2);
    } else {
        return std::sqrt(sy2);
    }
}

float* calcCos() {
    float dtr = PI / 180.0f;
    float* icos = new float[361];
    for (int32 i = 0; i < 361; ++i) {
        icos[i] = std::cos(i * dtr);
    }
    return icos;
}

float* calcSin() {
    float dtr = PI / 180.0f;
    float* isin = new float[361];
    for (int32 i = 0; i < 361; ++i) {
        isin[i] = std::sin(i * dtr);
    }
    return isin;
}

float FlatVectors::fastCos(float a) {
    return FlatVectors::icos[(int32)a];
}

float FlatVectors::fastSin(float a) {
    return FlatVectors::isin[(int32)a];
}

float* FlatVectors::icos = nullptr;

float* FlatVectors::isin = nullptr;

// Class

FlatVectors::FlatVectors() : paint(), uniform(), stroke() {
    lastStep = CLOSE;
    curVtxIndex = 0;
    curElmIndex = 0;
    curDrwIndex = 0;
    curDrawBeginVtxIndex = 0;
    curDrawBeginElmIndex = 0;
    curShapeBeginVtxIndex = 0;
    curShapeBeginElmIndex = 0;
    curPosX = 0;
    curPosY = 0;
    prevVtxIndex = 0;
    prevLineStartPosX = 0;
    prevLineStartPosY = 0;
    strokeStartPosX = 0;
    strokeStartPosY = 0;
    strokeFirstLineVtxIndex = 0;
    strokeFirstLinePosX = 0;
    strokeFirstLinePosY = 0;
    fontScale = 1;
    fontSpacing = 1;
    fontBlur = 0;
    curveJoin = 0;
    dashAmount = 0;
    dashPhase = 0;
    dashFill = 0;
    if (icos == nullptr) icos = calcCos();
    if (isin == nullptr) isin = calcSin();

    paint.transform[0] = 1;
    paint.transform[1] = 0;
    paint.transform[2] = 0;
    paint.transform[3] = 1;
    paint.transform[4] = 0;
    paint.transform[5] = 0;
    scale = maxscale(paint.transform);

    render = new FlatRender();
}

FlatVectors::~FlatVectors() {
    delete render;
}

bool FlatVectors::ensureSpace(int32 vertex, int32 elements) {
    if (curDrwIndex > 0 && (
                            (curVtxIndex + vertex) * 2 >= render->getMaxVertices() ||
                            (curElmIndex + elements) * 3 >= render->getMaxElements() ||
                            (curDrwIndex + 1) >= render->getMaxUniforms()
                            )
        ) {
        flush();
    }
    return !(
            (curVtxIndex + vertex) * 2 >= render->getMaxVertices() ||
            (curElmIndex + elements) * 3 >= render->getMaxElements() ||
            (curDrwIndex + 1) >= render->getMaxUniforms()
    );
}

int32 FlatVectors::addVertex(float x, float y, float u, float v) {
    vtx.push_back({x, y});
    uvs.push_back({u, v});
    return curVtxIndex++;
}

int32 FlatVectors::addTriangle(int32 elA, int32 elB, int32 elC) {
    elm.push_back({elA, elB, elC});
    return curElmIndex++;
}

bool FlatVectors::equals(float x, float y) {
    if (std::isnan(x) || std::isinf(x) || std::isnan(y) || std::isinf(y)) {
        return true;
    }
    return (std::abs(x - curPosX) < 0.5 / scale && std::abs(y - curPosY) < 0.5 / scale);
}

void FlatVectors::fillBegin() {
    curDrawBeginVtxIndex = curVtxIndex;
    curDrawBeginElmIndex = curElmIndex;
}

void FlatVectors::fillEnd() {
    if (lastStep == LINE || lastStep == MOVE) {
        fillClose();
    }

    if (curElmIndex == curDrawBeginElmIndex || curVtxIndex == curDrawBeginVtxIndex) {
        shapeDrawDiscard();
    } else {
        pushToRender();
    }
}

void FlatVectors::fillMoveTo(float x, float y) {
    if (lastStep == LINE || lastStep == MOVE) {
        fillClose();
    }

    lastStep = MOVE;
    curPosX = x;
    curPosY = y;
    curShapeBeginVtxIndex = curVtxIndex;
    curShapeBeginElmIndex = curElmIndex;
}

void FlatVectors::fillLineTo(float x, float y) {
    if (lastStep == CLOSE) {
        fillMoveTo(x, y);
    }

    if (equals(x, y)) return;

    lastStep = LINE;
    ensureSpace(1, 0);
    addVertex(curPosX, curPosY, 0, 0);
    curPosX = x;
    curPosY = y;
}

void FlatVectors::fillCurveTo(FlatVectors* context, double x, double y) {
    context->fillLineTo(static_cast<float>(x), static_cast<float>(y));
}

void FlatVectors::fillClose() {
    if (lastStep == CLOSE || lastStep == MOVE) {
        shapeDiscard();
        return;
    }

    lastStep = CLOSE;
    ensureSpace(1, 0);
    addVertex(curPosX, curPosY, 0, 0);

    if (curVtxIndex - curShapeBeginVtxIndex < 3) {
        shapeDiscard();
    } else {
        ensureSpace(0, curVtxIndex - curShapeBeginVtxIndex);
        int32 vtx0 = curShapeBeginVtxIndex;
        for (int32 i = curShapeBeginVtxIndex + 1; i < curVtxIndex - 1; i++) {
            addTriangle(vtx0, i, i + 1);
        }
    }
}

void FlatVectors::strokeBegin() {
    curDrawBeginVtxIndex = curVtxIndex;
    curDrawBeginElmIndex = curElmIndex;
    dashAmount = 0;
    dashPhase = 0;
    dashFill = 1;
}

void FlatVectors::strokeEnd() {
    if (lastStep == LINE || lastStep == MOVE) {
        strokeCap();
    }

    if (curElmIndex == curDrawBeginElmIndex || curVtxIndex == curDrawBeginVtxIndex) {
        shapeDrawDiscard();
    } else {
        pushToRender();
    }
}

void FlatVectors::strokeMoveTo(float x, float y) {
    if (lastStep == LINE || lastStep == MOVE) {
        strokeCap();
    }

    lastStep = MOVE;
    curPosX = x;
    curPosY = y;
    prevLineStartPosX = x;
    prevLineStartPosY = y;
    strokeStartPosX = x;
    strokeStartPosY = y;
    curShapeBeginVtxIndex = curVtxIndex;
    curShapeBeginElmIndex = curElmIndex;
}

void FlatVectors::strokeLineTo(float x, float y, bool curve) {
    if (lastStep == CLOSE) {
        strokeMoveTo(x, y);
    }

    if (equals(x, y)) return;

    float px1, py1, px2, py2, sx1, sy1, sx2, sy2 ;
    expandline(stroke.width, curPosX, curPosY, x, y,
               px1, py1, px2, py2, sx1, sy1, sx2, sy2);

    ensureSpace(4, 2);
    int32 el0 = addVertex(px1, py1, 0, 0);
    int32 el1 = addVertex(px2, py2, 0, 0);
    int32 el2 = addVertex(sx1, sy1, 0, 0);
    int32 el3 = addVertex(sx2, sy2, 0, 0);
    addTriangle(el0, el1, el2);
    addTriangle(el1, el3, el2);

    if (lastStep == LINE) {
        strokeJoin(prevVtxIndex, el0, prevLineStartPosX, prevLineStartPosY, curPosX, curPosY, x, y, curve);
    } else {
        strokeFirstLineVtxIndex = el0;
        strokeFirstLinePosX = x;
        strokeFirstLinePosY = y;
    }

    prevVtxIndex = el0;
    prevLineStartPosX = curPosX;
    prevLineStartPosY = curPosY;
    curPosX = x;
    curPosY = y;
    lastStep = LINE;
}

void FlatVectors::strokeCurveTo(FlatVectors* context, double x, double y) {
    if (context->stroke.dash.empty()) {
        context->strokeLineTo(static_cast<float>(x), static_cast<float>(y), true);
    } else {
        context->strokeDashTo(static_cast<float>(x), static_cast<float>(y), true);
    }
}

void FlatVectors::strokeCap() {
    if (lastStep == MOVE) {
        if (stroke.dash.empty()) {
            if (stroke.cap == CAP_SQUARE) {
                float w = stroke.width * 0.5;
                int32 el0 = addVertex(curPosX - w, curPosY - w, 0, 0);
                int32 el1 = addVertex(curPosX + w, curPosY - w, 0, 0);
                int32 el2 = addVertex(curPosX + w, curPosY + w, 0, 0);
                int32 el3 = addVertex(curPosX - w, curPosY + w, 0, 0);
                addTriangle(el0, el1, el2);
                addTriangle(el0, el2, el3);
            } else if (stroke.cap == CAP_ROUND) {
                float w = stroke.width * 0.5;
                strokeRound({curPosX - w, curPosY}, {curPosX + w, curPosY}, {curPosX, curPosY});
                strokeRound({curPosX + w, curPosY}, {curPosX - w, curPosY}, {curPosX, curPosY});
            }
        }

    } else if (stroke.cap == fvCap::CAP_BUTT) {
        // Nothing
    } else if (stroke.cap == fvCap::CAP_SQUARE) {
        // Extrude Start + End
        fvPoint p0 = vtx[strokeFirstLineVtxIndex];
        fvPoint p1 = vtx[strokeFirstLineVtxIndex + 1];
        fvPoint p2 = vtx[strokeFirstLineVtxIndex + 2];
        fvPoint p3 = vtx[strokeFirstLineVtxIndex + 3];

        float w = stroke.width * 0.5;
        float det = std::sqrt((p0.x - p2.x) * (p0.x - p2.x) + (p0.y - p2.y) * (p0.y - p2.y));
        vtx[strokeFirstLineVtxIndex] = {
                (p0.x - p2.x) / det * w + p0.x,
                (p0.y - p2.y) / det * w + p0.y
        };
        vtx[strokeFirstLineVtxIndex + 1] = {
                (p0.x - p2.x) / det * w + p1.x,
                (p0.y - p2.y) / det * w + p1.y
        };

        p0 = vtx[prevVtxIndex + 2];
        p1 = vtx[prevVtxIndex + 3];
        p2 = vtx[prevVtxIndex];
        p3 = vtx[prevVtxIndex + 1];
        det = std::sqrt((p0.x - p2.x) * (p0.x - p2.x) + (p0.y - p2.y) * (p0.y - p2.y));
        vtx[prevVtxIndex + 2] = {
                (p0.x - p2.x) / det * w + p0.x,
                (p0.y - p2.y) / det * w + p0.y
        };
        vtx[prevVtxIndex + 3] = {
                (p0.x - p2.x) / det * w + p1.x,
                (p0.y - p2.y) / det * w + p1.y
        };
    } else if (stroke.cap == fvCap::CAP_ROUND) {
        fvPoint from = vtx[strokeFirstLineVtxIndex + 1];
        fvPoint to = vtx[strokeFirstLineVtxIndex];
        fvPoint center = {(from.x + to.x) * 0.5f, (from.y + to.y) * 0.5f};
        strokeRound(from, to, center);

        from = vtx[prevVtxIndex + 2];
        to = vtx[prevVtxIndex + 3];
        center = {(from.x + to.x) * 0.5f, (from.y + to.y) * 0.5f};
        strokeRound(from, to, center);
    }
}

void FlatVectors::strokeJoin(int32 v0, int32 v1, float x1, float y1, float x2, float y2, float x3, float y3, bool cruve) {
    float det = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);

    fvJoin join = stroke.join;
    if (cruve && curveJoin == 0) {
        fvPoint p1 = vtx[v0 + 2];
        fvPoint p2 = vtx[v0 + 3];
        fvPoint p3 = vtx[v1];
        fvPoint p4 = vtx[v1 + 1];
        float dot_product = (p1.x - p2.x) / stroke.width * (p3.x - p4.x) / stroke.width +
                            (p1.y - p2.y) / stroke.width * (p3.y - p4.y) / stroke.width;
        if (dot_product > 0.7) {
            join = JOIN_BEVEL;
        } else {
            join = JOIN_ROUND;
        }
    }
    curveJoin = 0;

    if (join == fvJoin::JOIN_BEVEL) {
        strokeBevel(det, v0, v1);

    } else if (join == fvJoin::JOIN_MITER) {

        fvPoint c = {x2, y2};
        fvPoint mt1;
        float m1;
        if (det >= 0) {
            m1 = miterPoint(vtx[v0], vtx[v0 + 2], vtx[v1], vtx[v1 + 2], mt1);
        } else {
            m1 = miterPoint(vtx[v0 + 1], vtx[v0 + 3], vtx[v1 + 1], vtx[v1 + 3], mt1);
        }

        float limit = stroke.miter * stroke.miter;
        if (m1 != 0 && (mt1.x - c.x) * (mt1.x - c.x) + (mt1.y - c.y) * (mt1.y - c.y) <= limit) {
            if (det >= 0) {
                vtx[v0 + 2] = mt1;
                vtx[v1] = mt1;
            } else {
                vtx[v0 + 3] = mt1;
                vtx[v1 + 1] = mt1;
            }
        } else {
            strokeBevel(det, v0, v1);
        }

    } else if (join == fvJoin::JOIN_ROUND) {
        fvPoint c = {x2, y2};
        fvPoint from = det >= 0 ? vtx[v0 + 2] : vtx[v1 + 1];
        fvPoint to = det >= 0 ? vtx[v1] : vtx[v0 + 3];
        strokeRound(from, to, c);
    }
}

void FlatVectors::strokeClose() {
    if (lastStep == MOVE) {
        strokeCap();

    } else if (lastStep == LINE) {
        strokeLineTo(strokeStartPosX, strokeStartPosY, false);
        strokeJoin(prevVtxIndex, strokeFirstLineVtxIndex,
                   prevLineStartPosX, prevLineStartPosY, curPosX, curPosY, strokeFirstLinePosX, strokeFirstLinePosY, false);
    }

    lastStep = CLOSE;
    if (curVtxIndex - curShapeBeginVtxIndex < 3) {
        shapeDiscard();
    }
}

void FlatVectors::strokeBevel(float det, int32 v0, int32 v1) {
    if (det >= 0) {
        addTriangle(v0 + 2, v1, v1 + 1);
    } else {
        addTriangle(v0 + 2, v1 + 1, v0 + 3);
    }
}

void FlatVectors::strokeRound(fvPoint from, fvPoint to, fvPoint center) {
    float w = stroke.width * 0.5;

    float a1 = std::atan2(from.y - center.y, from.x - center.x);
    float a2 = std::atan2(to.y - center.y, to.x - center.x);

    float ang = a2 - a1;
    if (ang < -M_PI) ang += 2 * M_PI;
    if (ang < 0) w = -w;

    float v = (std::abs(w * scale) + 10);
    v = (std::abs(ang) / PI2) * (v < 10 ? 10 : v > 64 ? 64 : v);
    int32 n = (int32) std::ceil(v < 2 ? 2 : v);

    int32 pastEl = addVertex(center.x + std::cos(a1) * w, center.y + std::sin(a1) * w, 0, 0);
    int32 cEl = addVertex(center.x, center.y, 0, 0);
    for (int32 i = 1; i <= n; i += 1) {
        float u = i / (float) n;
        float ca = a1 + ang * u;

        int32 el = addVertex(center.x + std::cos(ca) * w, center.y + std::sin(ca) * w, 0, 0);
        addTriangle(cEl, pastEl, el);
        pastEl = el;
    }
}

void FlatVectors::strokeDashTo(float x, float y, bool curve) {
    if (lastStep == CLOSE) {
        strokeMoveTo(x, y);
    }

    if (equals(x, y)) return;

    float xOff = x - curPosX;
    float yOff = y - curPosY;
    float len = std::sqrt(xOff * xOff + yOff * yOff);
    xOff /= len;
    yOff /= len;
    while (dashAmount + len > stroke.dash[dashPhase]) {
        float step = stroke.dash[dashPhase] - dashAmount;
        dashAmount = 0;
        if (dashFill == 1) {
            strokeLineTo(curPosX + xOff * step, curPosY + yOff * step, curve);
        } else {
            strokeMoveTo(curPosX + xOff * step, curPosY + yOff * step);
        }
        dashFill = !dashFill;
        len -= step;
        dashPhase++;
        if (dashPhase >= stroke.dash.size()) {
            dashPhase = 0;
        }
    }
    dashAmount += len;
    if (dashFill == 1) {
        strokeLineTo(x, y, curve);
    } else {
        strokeMoveTo(x, y);
    }
}

void FlatVectors::shapeDiscard() {
    vtx.resize(curShapeBeginVtxIndex);
    uvs.resize(curShapeBeginVtxIndex);
    elm.resize(curShapeBeginElmIndex);
    curVtxIndex = curShapeBeginVtxIndex;
    curElmIndex = curShapeBeginElmIndex;
}

void FlatVectors::shapeDrawDiscard() {
    vtx.resize(curDrawBeginVtxIndex);
    uvs.resize(curDrawBeginVtxIndex);
    elm.resize(curDrawBeginElmIndex);
    curVtxIndex = curDrawBeginVtxIndex;
    curElmIndex = curDrawBeginElmIndex;
}

void FlatVectors::pushToRender() {
    paint.elements = curElmIndex;
    paint.vertices = curVtxIndex;
    paint.convex = paint.pathOp == CONVEX;
    draws.push_back(paint);

    fvUniform push = uniform;

    if (paint.pathOp == TEXT) {
        if (paint.font->getFont()->isSdf()) {
            push.extra[3] = fontBlur;
        } else {
            push.extra[3] = -1;
        }

        if (push.type == 0) {
            push.type = 2;
        } else if (push.type == 1) {
            push.type = 3;
        }
    }

    uniforms.resize((curDrwIndex + 1) * render->renderAlign());
    void* memory = uniforms.data() + (curDrwIndex * render->renderAlign());
    memcpy(memory, &push, sizeof(fvUniform));

    curDrwIndex++;
}

bool FlatVectors::debug = false;

void FlatVectors::setDebug(bool debug) {
    FlatVectors::debug = debug;
}

FlatRender* FlatVectors::getRender() {
    return render;
}

void FlatVectors::beginFrame(int32 width, int32 height) {
    render->begin(width, height, debug);
}

void FlatVectors::endFrame() {
    flush();

    curVtxIndex = 0;
    curElmIndex = 0;
    curDrwIndex = 0;

    curDrawBeginVtxIndex = 0;
    curDrawBeginElmIndex = 0;
    curShapeBeginVtxIndex = 0;
    curShapeBeginElmIndex = 0;

    strokeFirstLineVtxIndex = 0;
    prevVtxIndex = 0;

    elm.resize(0);
    vtx.resize(0);
    uvs.resize(0);
    draws.resize(0);
    uniforms.resize(0);

    render->end();
}

void FlatVectors::flush() {
    if (curDrwIndex == 0) {
        return;
    }
    int32 lastElm = paint.elements;
    int32 lastVtx = paint.vertices;
    if (lastElm > 0) {
        render->flush(
                draws.data(), uniforms.data(), curDrwIndex,
                reinterpret_cast<int32 *>(elm.data()), lastElm * 3,
                reinterpret_cast<float *>(vtx.data()), reinterpret_cast<float *>(uvs.data()), lastVtx * 2
        );
        for (int32 i = lastElm; i < curElmIndex; ++i) {
            elm[i].a -= lastVtx;
            elm[i].b -= lastVtx;
            elm[i].c -= lastVtx;
        }

        curVtxIndex -= lastVtx;
        curElmIndex -= lastElm;
        curDrwIndex = 0;

        curDrawBeginVtxIndex -= lastVtx;
        curDrawBeginElmIndex -= lastElm;
        curShapeBeginVtxIndex -= lastVtx;
        curShapeBeginElmIndex -= lastElm;

        strokeFirstLineVtxIndex -= lastVtx;
        prevVtxIndex -= lastVtx;

        elm.erase(elm.begin(), elm.begin() + lastElm);
        vtx.erase(vtx.begin(), vtx.begin() + lastVtx);
        uvs.erase(uvs.begin(), uvs.begin() + lastVtx);
        draws.resize(0);
        uniforms.resize(0);
    }

}

void FlatVectors::setColor(fvUniform& color, int32 img) {
    uniform = color;

    inverseMat(uniform.colorMat, uniform.colorMat);
    affineToMat3(uniform.colorMat, uniform.colorMat);

    inverseMat(uniform.imageMat, uniform.imageMat);
    affineToMat3(uniform.imageMat, uniform.imageMat);

    paint.image0 = img;
}

void FlatVectors::setStroke(FlatStroke& stroke) {
    this->stroke = stroke;
    this->stroke.miter = this->stroke.width * this->stroke.miter * 0.5;
}

void FlatVectors::setTransform(float m00, float m10, float m01, float m11, float m02, float m12) {
    paint.transform[0] = m00;
    paint.transform[1] = m10;
    paint.transform[2] = m01;
    paint.transform[3] = m11;
    paint.transform[4] = m02;
    paint.transform[5] = m12;
    scale = maxscale(paint.transform);
}

void FlatVectors::setAntiAliasing(int32 enabled) {
    paint.antiAlias = enabled;
}

void FlatVectors::setFont(FlatFontRender *fontRender) {
    paint.font = fontRender;
}

void FlatVectors::setFontScale(float fscale) {
    fontScale = fscale;
}

void FlatVectors::setFontSpacing(float fspacing) {
    fontSpacing = fspacing;
}

void FlatVectors::setFontBlur(float fblur) {
    fontBlur = fblur < 0 ? 0 : fblur > 1 ? 1 : fblur;
}

void FlatVectors::begin(fvPathOp pathOp, fvWindingRule pathRule) {
    paint.pathOp = pathOp;
    paint.pathRule = pathRule;
    lastStep = CLOSE;
    if (paint.pathOp == STROKE) {
        strokeBegin();
    } else {
        fillBegin();
    }
    ensureSpace(0, 0);
}

void FlatVectors::end() {
    if (paint.pathOp == STROKE) {
        strokeEnd();
    } else {
        fillEnd();
    }
    ensureSpace(0, 0);
}

void FlatVectors::moveTo(float x, float y) {
    if (paint.pathOp == STROKE) {
        strokeMoveTo(x, y);
    } else {
        fillMoveTo(x, y);
    }
}

void FlatVectors::lineTo(float x, float y) {
    if (paint.pathOp == STROKE) {
        if (stroke.dash.empty()) {
            strokeLineTo(x, y, false);
        } else {
            strokeDashTo(x, y, false);
        }
    } else {
        fillLineTo(x, y);
    }
}

void FlatVectors::quadTo(float cx, float cy, float x, float y) {
    if (equals(cx, cy) && equals(x, y)) return;

    curveJoin = 1;
    if (paint.pathOp == STROKE) {
        FlatCurves::tessQuad(scale * 2, curPosX, curPosY, cx, cy, x, y, this, &strokeCurveTo);
    } else {
        FlatCurves::tessQuad(scale * 2, curPosX, curPosY, cx, cy, x, y, this, &fillCurveTo);
    }
    curveJoin = 0;
}

void FlatVectors::cubicTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
    if (equals(cx1, cy1) && equals(cx2, cy2) && equals(x, y)) return;

    curveJoin = 1;
    if (paint.pathOp == STROKE) {
        FlatCurves::tessCubic(scale * 2, curPosX, curPosY, cx1, cy1, cx2, cy2, x, y, this, &strokeCurveTo);
    } else {
        FlatCurves::tessCubic(scale * 2, curPosX, curPosY, cx1, cy1, cx2, cy2, x, y, this, &fillCurveTo);
    }
    curveJoin = 0;
}

void FlatVectors::close() {
    if (paint.pathOp == STROKE) {
        strokeClose();
    } else {
        fillClose();
    }
}

void FlatVectors::clearClip(bool clip) {
    render->clearClip(clip);
}

void FlatVectors::text(const char* str, int32 strLen, float x, float y, float maxWidth, float maxHeight) {
    if (maxWidth == 0) maxWidth = 99999;
    else maxWidth = x + maxWidth;
    if (maxHeight == 0) maxHeight = 99999;
    else maxHeight = y + maxHeight;

    FlatFontRender* fontRender = paint.font;
    FlatFont* font = fontRender->getFont();
    float scl = fontScale;
    float spc = fontSpacing;

    begin(fvPathOp::TEXT, fvWindingRule::EVEN_ODD);

    int32 p = 0, i = 0, f = 0;
    uint32 chr = 0, prev = 0;
    while (FlatText::utf8loop(str, strLen, i, chr)) {
        if (chr == '\n') continue;

        fvPoint uv;
        int32 recreate;
        fvGlyph& glyph = font->getGlyphRendered(fontRender, chr, &uv, &recreate);
        if (recreate == 2) {
            end();
            flush();

            font->getGlyphRendered(fontRender, chr, &uv, &recreate);
            begin(fvPathOp::TEXT, fvWindingRule::EVEN_ODD);
        }

        float kern = (f ? font->getKerning(prev, chr) : 0);
        float advance = (glyph.advance + kern) * (scl * spc);

        float px = x + kern * scl * spc;
        if (uv.x > -1) {
            float x1 = px + glyph.x * scl;
            float y1 = y + glyph.y * scl;
            float x2 = x1 + glyph.w * scl;
            float y2 = y1 + glyph.h * scl;

            if (x1 < maxWidth && y1 < maxHeight) {
                float uvW = glyph.w;
                float uvH = glyph.h;
                if (x2 > maxWidth) {
                    float wb = x2 - x1;
                    x2 = maxWidth;
                    float wa = x2 - x1;
                    uvW *= wa / wb;
                }
                if (y2 > maxHeight) {
                    float hb = y2 - y1;
                    y2 = maxHeight;
                    float ha = y2 - y1;
                    uvH *= ha / hb;
                }

                if (!ensureSpace(4, 2)) {
                    end();
                    flush();
                    begin(fvPathOp::TEXT, fvWindingRule::EVEN_ODD);
                }
                int32 el0 = addVertex(x1, y1, uv.x, uv.y);
                int32 el1 = addVertex(x2, y1, uv.x + uvW, uv.y);
                int32 el2 = addVertex(x2, y2, uv.x + uvW, uv.y + uvH);
                int32 el3 = addVertex(x1, y2, uv.x, uv.y + uvH);
                addTriangle(el0, el1, el2);
                addTriangle(el0, el2, el3);
            }
        }
        x += advance;
        if (x > maxWidth) {
            break;
        }

        prev = chr;
        f = 1;
        p = i;
    }
    end();
}

void FlatVectors::rect(float x, float y, float width, float height, bool fill) {
    begin(fill ? fvPathOp::CONVEX : fvPathOp::STROKE, fvWindingRule::EVEN_ODD);
    moveTo(x, y);
    lineTo(x + width, y);
    lineTo(x + width, y + height);
    lineTo(x, y + height);
    close();
    end();
}

void FlatVectors::ellipse(float x, float y, float width, float height, bool fill) {
    begin(fill ? fvPathOp::CONVEX : fvPathOp::STROKE, fvWindingRule::EVEN_ODD);
    float points = scale * std::sqrt(std::max(width, height) * 0.5) * PI2;
    points = std::ceil((points < 16 ? 16 : points > 128 ? 128 : points));
    int32 n = (int32) points;

    float dtr = PI / 180.0f;
    float hw = width / 2.0f, hh = height / 2.0f;
    float xc = x + hw, yc = y + hh;

    for (int32 i = 0; i < n ; i++) {
        float a = (i / (float) n * 360) * dtr;
        if (i == 0) {
            moveTo(xc + std::cos(a) * hw, yc - std::sin(a) * hh);
        } else {
            lineTo(xc + std::cos(a) * hw, yc - std::sin(a) * hh);
        }
    }
    close();
    end();
}

void FlatVectors::roundRect(float x, float y, float width, float height, float c1, float c2, float c3, float c4, bool fill) {
    begin(fill ? fvPathOp::CONVEX : fvPathOp::STROKE, fvWindingRule::EVEN_ODD);

    float max = scale * std::min(width, height) / 2;
    c1 = std::min(max, c1);
    c2 = std::min(max, c2);
    c3 = std::min(max, c3);
    c4 = std::min(max, c4);

    float xc = x + c1, yc = y + c1;
    if (c1 > 0.5 / scale) {
        int32 n = std::ceil(4 + (8 * (c1 * scale - 4) / 32.0));
        for (int32 i = 0; i <= n; i ++) {
            float a = (90 + (90 * i / (float)n));
            if (i == 0) {
                moveTo(xc + fastCos(a) * c1, yc - fastSin(a) * c1);
            } else {
                lineTo(xc + fastCos(a) * c1, yc - fastSin(a) * c1);
            }
        }
    } else {
        moveTo(x, y);
    }
    xc = x + c4, yc = y + height - c4;
    if (c4 > 0.5 / scale) {
        int32 n = std::ceil(4 + (8 * (c4 * scale - 4) / 32.0));
        for (int32 i = 0; i <= n; i ++) {
            float a = (180 + (90 * i / (float)n));
            lineTo(xc + fastCos(a) * c4, yc - fastSin(a) * c4);
        }
    } else {
        lineTo(x, y + height);
    }
    xc = x + width - c3, yc = y + height - c3;
    if (c3 > 0.5 / scale) {
        int32 n = std::ceil(4 + (8 * (c3 * scale - 4) / 32.0));
        for (int32 i = 0; i <= n; i ++) {
            float a = (270 + (90 * i / (float)n));
            lineTo(xc + fastCos(a) * c3, yc - fastSin(a) * c3);
        }
    } else {
        lineTo(x + width, y + height);
    }
    xc = x + width - c2, yc = y + c2;
    if (c2 > 0.5 / scale) {
        int32 n = std::ceil(4 + (8 * (c2 * scale - 4) / 32.0));
        for (int32 i = 0; i <= n; i ++) {
            float a = (90 * i / (float)n);
            lineTo(xc + fastCos(a) * c2, yc - fastSin(a) * c2);
        }
    } else {
        lineTo(x + width, y);
    }
    close();
    end();
}
