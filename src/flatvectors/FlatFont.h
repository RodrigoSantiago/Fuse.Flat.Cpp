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

    int sdf;
    float scale;
    float size;
    float ascent;
    float descent;
    float lineGap;
    float height;

    bool coded;

    int cellW;
    int cellH;
    int glyphCount;
    fvGlyph* glyphs;

public:
    FlatFont(const void* data, long int length, float size, int sdf);
    ~FlatFont();

private:
    void loadGlyph(int glyphIndex);

    void renderGlyph(int glyphIndex);

public:
    bool isValid();

    bool isSdf();

    int getCellW();

    int getCellH();

    int getGlyphCount();

    void getAllCodePoints(std::vector<long int>& codepoints);

    void getGlyphData(long codePoint, float* data);

    void getGlyphShape(long codePoint, std::vector<float> &polygon);

    void getMetrics(float* ascender, float* descender, float* height, float* lineGap, int* glyphCount);

    fvGlyph& getGlyph(long unicode);

    fvGlyph& getGlyphRendered(FlatFontRender* font, long unicode, fvPoint* uv, int* recreate);

    float getKerning(long unicode1, long unicode2);

    float getTextWidth(const char* str, int strLen, float scale, float spacing);

    void getOffset(const char* str, int strLen, float scale, float spacing, float cursorX, int half, float* index, float* width);
};


#endif //FLAT_FLATFONT_H
