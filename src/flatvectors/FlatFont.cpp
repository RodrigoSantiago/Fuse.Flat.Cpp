//
// Created by Rodrigo on 15/03/2025.
//

#include "FlatFont.h"
#include "FlatRender.h"
#include "FlatFontRender.h"
#include "FlatText.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define PADDING 8
#define PADDING2 16

int _maxCodePoint(stbtt_fontinfo *info){
    stbtt_uint8 *data = info->data;
    stbtt_uint32 index_map = info->index_map;

    stbtt_uint16 format = ttUSHORT(data + index_map + 0);
    if (format == 0) { // apple byte encoding
        stbtt_int32 bytes = ttUSHORT(data + index_map + 2);
        return bytes-6;
    } else if (format == 6) {
        stbtt_uint32 first = ttUSHORT(data + index_map + 6);
        stbtt_uint32 count = ttUSHORT(data + index_map + 8);
        return first + count;
    } else if (format == 2) {
        return 0;
    } else if (format == 4) {
        return 0xffff;
    } else if (format == 12 || format == 13) {
        return 0;
    }
    return 0;
}

// Class

FlatFont::FlatFont(const void *data, long length, float size, int sdf) : valid(false), glyphs(0), glyphCount(0), coded(false) {
    this->size = size;
    this->sdf = sdf;
    this->data = new unsigned char[length];
    memcpy(this->data, data, length);

    if (stbtt_InitFont(&info, this->data, 0)) {
        this->valid = true;
        this->glyphCount = info.numGlyphs;
        this->glyphs = new fvGlyph[glyphCount];
        for (int i = 0; i < glyphCount; ++i) {
            glyphs[i] = {};
        }

        this->scale = stbtt_ScaleForPixelHeight(&info, size);

        int a, d, g;
        stbtt_GetFontVMetrics(&info, &a, &d, &g);
        this->ascent = a * this->scale;
        this->descent = d * this->scale;
        this->lineGap = g * this->scale;
        this->height = (a - d) * this->scale;

        int x, y, w, h;
        stbtt_GetFontBoundingBox(&info, &x, &y, &w, &h);
        cellW = ceil((w - x) * scale) + PADDING2;
        cellH = ceil((h - y) * scale) + PADDING2;
        if (cellW > ceil(this->height * 1.01 + PADDING2) * 2) {
            cellW = ceil(this->height * 1.01 + PADDING2);
        }
        if (cellH > ceil(this->height * 1.01 + PADDING2) * 2) {
            cellH = ceil(this->height * 1.01 + PADDING2);
        }
    }
}

FlatFont::~FlatFont() {
    for (int i = 0; i < glyphCount; ++i) {
        delete[] glyphs[i].cell;
    }
    delete[] data;
    delete[] glyphs;
}

// Private

void FlatFont::loadGlyph(int glyphIndex) {
    int c_x1, c_y1, c_x2, c_y2;
    stbtt_GetGlyphBitmapBox(&info, glyphIndex, scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

    int ax, lb;
    stbtt_GetGlyphHMetrics(&info, glyphIndex, &ax, &lb);

    fvGlyph& glyph = glyphs[glyphIndex];
    glyph.enabled = 1;
    glyph.advance = ax * scale;

    if (c_x2 - c_x1 <= 0 || c_y2 - c_y1 <= 0) {
        glyph.x = 0;
        glyph.y = 0;
        glyph.w = 0;
        glyph.h = 0;
    } else {
        glyph.x = c_x1 - PADDING;
        glyph.y = c_y1 + ascent - PADDING;
        glyph.w = c_x2 - c_x1 + PADDING2;
        glyph.h = c_y2 - c_y1 + PADDING2;
    }
}

void FlatFont::renderGlyph(int glyphIndex) {
    fvGlyph& glyph = glyphs[glyphIndex];

    if (glyph.cell == nullptr) {
        int width = (int) ceil(glyph.w);
        int height = (int) ceil(glyph.h);

        int cellW = width <= this->cellW ? 1 : static_cast<int>(ceil(width / static_cast<double>(this->cellW)));
        int cellH = height <= this->cellH ? 1 : static_cast<int>(ceil(height / static_cast<double>(this->cellH)));
        cellW *= this->cellW;
        cellH *= this->cellH;

        int len = cellW * cellH;
        unsigned char *img = new unsigned char[len];
        for (int i = 0; i < len; ++i) {
            img[i] = 0;
        }

        if (sdf) {
            int w, h, xof, yof;
            unsigned char *bmap = stbtt_GetGlyphSDF(&info, scale, glyphIndex, PADDING, 128, 16
                    , &w, &h, &xof, &yof);
            if (bmap != 0) {
                for (int y = 0; y < height && y < h; y++) {
                    for (int x = 0; x < width && x < w; x++) {
                        img[x + y * cellW] = bmap[x + y * w];
                    }
                }
                stbtt_FreeSDF(bmap, &info.userdata);
            }
        } else {
            stbtt_MakeGlyphBitmap(&info, img + (PADDING * cellW)/*y*/ + PADDING/*x*/
                    , width - PADDING2
                    , height - PADDING2
                    , cellW
                    , scale, scale, glyphIndex);
        }
        glyph.cell = img;
    }
}

// Public

bool FlatFont::isValid() {
    return valid;
}

bool FlatFont::isSdf() {
    return sdf == 1;
}

int FlatFont::getCellW() {
    return this->cellW;
}

int FlatFont::getCellH() {
    return this->cellH;
}

int FlatFont::getGlyphCount() {
    return this->glyphCount;
}

void FlatFont::getAllCodePoints(std::vector<long int>& codepoints) {
    if (!coded) {
        int max = _maxCodePoint(&info);
        if (max == 0) {
            max = 0x10FFFF;
        }

        for (long int codePoint = 0; codePoint <= max; codePoint++) {
            int glyphIndex = stbtt_FindGlyphIndex(&info, codePoint);
            if (glyphIndex != 0) {
                glyphs[glyphIndex].unicode = codePoint;
            }
        }
        coded = true;
    }

    codepoints.reserve(glyphCount);
    for (int i = 0; i < glyphCount; ++i) {
        codepoints[i] = glyphs[i].unicode;
    }
}

void FlatFont::getGlyphData(long codePoint, float* data) {
    fvGlyph& glyph = getGlyph(codePoint);
    data[0] = glyph.advance;
    data[1] = glyph.x;
    data[2] = glyph.y;
    data[3] = glyph.w;
    data[4] = glyph.h;
}

void FlatFont::getGlyphShape(long unicode, std::vector<float> &polygon) {
    int glyphIndex = stbtt_FindGlyphIndex(&info, unicode);

    stbtt_vertex* vertices;
    int num = stbtt_GetGlyphShape(&info, glyphIndex, &vertices);

    if (num > 0) {
        polygon.reserve(num * 7);
        for (int i = 0; i < num; ++i) {
            const stbtt_vertex* v = &vertices[i];

            switch (v->type) {
                case STBTT_vmove:
                    polygon.push_back(0);
                    polygon.push_back(v->x / size);
                    polygon.push_back(-v->y / size);
                    break;
                case STBTT_vline:
                    polygon.push_back(1);
                    polygon.push_back(v->x / size);
                    polygon.push_back(-v->y / size);
                    break;
                case STBTT_vcurve:
                    polygon.push_back(2);
                    polygon.push_back(v->cx / size);
                    polygon.push_back(-v->cy / size);
                    polygon.push_back(v->x / size);
                    polygon.push_back(-v->y / size);
                    break;
                case STBTT_vcubic:
                    polygon.push_back(3);
                    polygon.push_back(v->cx1 / size);
                    polygon.push_back(-v->cy1 / size);
                    polygon.push_back(v->cx / size);
                    polygon.push_back(-v->cy / size);
                    polygon.push_back(v->x / size);
                    polygon.push_back(-v->y / size);
                    break;
            }
        }
        stbtt_FreeShape(&info, vertices);
    }
}

void FlatFont::getMetrics(float *ascender, float *descender, float *height, float *lineGap, int *glyphCount) {
    if (ascender != 0) *ascender = this->ascent;
    if (descender != 0) *descender = this->descent;
    if (height != 0) *height = this->height;
    if (lineGap != 0) *lineGap = this->lineGap;
    if (glyphCount != 0) *glyphCount = this->glyphCount;
}

fvGlyph& FlatFont::getGlyph(long unicode) {
    int glyphIndex = stbtt_FindGlyphIndex(&info, unicode);
    fvGlyph& glyph = glyphs[glyphIndex];

    if (!glyph.enabled) {
        loadGlyph(glyphIndex);
    }

    return glyph;
}

float FlatFont::getKerning(long unicode1, long unicode2) {

    return stbtt_GetCodepointKernAdvance(&info, unicode1, unicode2) * scale;
}

fvGlyph& FlatFont::getGlyphRendered(FlatFontRender *font, long unicode, fvPoint *uv, int *recreate) {
    int glyphIndex = stbtt_FindGlyphIndex(&info, unicode);

    fvGlyph& glyph = glyphs[glyphIndex];
    if (!glyph.enabled) {
        loadGlyph(glyphIndex);
    }

    if (!font->isGlyphRendered(glyphIndex)) {
        if (glyph.cell == nullptr) {
            renderGlyph(glyphIndex);
        }

        *recreate = font->renderGlyph(glyph, glyphIndex);
    }

    (*uv) = font->getUv(glyphIndex);
    return glyph;
}

float FlatFont::getTextWidth(const char *str, int strLen, float scale, float spacing) {
    float scl = scale * spacing;

    float w = 0;
    int i = 0, f = 0;
    unsigned long chr = 0, prev = 0;
    while (FlatText::utf8loop(str, strLen, i, chr)) {
        if (chr != '\n') {
            fvGlyph &glyph = getGlyph(chr);

            w += ceil((glyph.advance + (f ? getKerning(prev, chr) : 0)) * scl);
            prev = chr;
            f = 1;
        }
    }
    return w;
}

void FlatFont::getOffset(const char *str, int strLen, float scale, float spacing, float cursorX, int half,
                         float *index, float *width) {
    float scl = scale * spacing;

    float w = 0;
    int i = 0, f = 0, pi = 0;
    unsigned long chr = 0, pchr = 0;
    while (FlatText::utf8loop(str, strLen, i, chr)) {
        if (chr == '\n') continue;

        fvGlyph &glyph = getGlyph(chr);

        float advance = ceil((glyph.advance + (f ? getKerning(pchr, chr) : 0)) * scl);
        if (w + advance > cursorX) {
            if (cursorX <= w + advance * 0.5) {
                *width = w;
                *index = pi;
            } else if (half) {
                *width = w + advance;
                *index = i;
            } else {
                *width = w;
                *index = pi;
            }
            return;
        }
        w += advance;
        pchr = chr;
        pi = i;
        f = 1;
    }
    *width = w;
    *index = pi;
}