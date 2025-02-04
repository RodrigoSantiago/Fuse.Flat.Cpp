//
// Created by Rodrigo on 30/10/2018.
//

#ifndef FLATVECTORS_FONT_H
#define FLATVECTORS_FONT_H

#include <flatvectors.h>

void* fontCreate(const void* data, long int length, float size, int sdf);

void fontDestroy(void* ctx);

void fontGetData(void* ctx, fvFont* ft);

void fontLoadGlyphs(void* ctx, const char* str, int strLen, int state);

void fontLoadAllGlyphs(void* ctx);

void fontGetMetrics(void* ctx, float* ascender, float* descender, float* height, float* lineGap);

fvGlyph& fontGlyph(void* ctx, long unicode);

fvGlyph& fontGlyphRendered(void* ctx, fvFont* ft, long unicode);

float fontKerning(void* ctx, long unicode1, long unicode2);

#endif //FLATVECTORS_FONT_H
