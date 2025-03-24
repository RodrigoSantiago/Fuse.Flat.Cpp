//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATFONT_H
#define FLAT_FLATFONT_H

#include <vector>
#include "FlatVectorsBase.h"
#include "stb_truetype.h"

class FlatFont {
    unsigned char* data;
    stbtt_fontinfo info;
    bool valid;

    int32 sdf;
    float scale;
    float size;
    float ascent;
    float descent;
    float lineGap;
    float height;

    bool coded;

    int32 cellW;
    int32 cellH;
    int32 glyphCount;
    fvGlyph* glyphs;

public:
    FlatFont(const void* data, int32 length, float size, int32 sdf);
    ~FlatFont();

private:
    void loadGlyph(int32 glyphIndex);

    void renderGlyph(int32 glyphIndex);

public:
    bool isValid();

    bool isSdf();

    int32 getCellW();

    int32 getCellH();

    int32 getGlyphCount();

    void getAllCodePoints(std::vector<int32>& codepoints);

    void getGlyphData(int32 codePoint, float* data);

    void getGlyphShape(int32 codePoint, std::vector<float> &polygon);

    void getMetrics(float* ascender, float* descender, float* height, float* lineGap, int32* glyphCount);

    fvGlyph& getGlyph(int32 unicode);

    fvGlyph& getGlyphRendered(FlatFontRender* font, int32 unicode, fvPoint* uv, int32* recreate);

    float getKerning(int32 unicode1, int32 unicode2);

    float getTextWidth(const char* str, int32 strLen, float scale, float spacing);

    void getOffset(const char* str, int32 strLen, float scale, float spacing, float cursorX, int32 half, float* index, float* width);

    void getOffsetSpace(const char* str, int32 strLen, float scale, float spacing, float cursorX, float* index, float* width);

    int countLineWrap(const char* str, int32 strLen, float scale, float spacing, float maxWidth);
};


#endif //FLAT_FLATFONT_H
