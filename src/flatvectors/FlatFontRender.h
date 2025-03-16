//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATFONTRENDER_H
#define FLAT_FLATFONTRENDER_H

#include "FlatVectorsBase.h"

class FlatFontRender {
    FlatFont* font;
    FlatRender* render;

    FlatPack* pack;
    fvPoint* renderState;
    unsigned long imageID;

public:
    FlatFontRender(FlatFont* font, FlatRender* render);

    ~FlatFontRender();

    FlatFont* getFont();

    int renderGlyph(fvGlyph& glyph, int glyphIndex);

    bool isGlyphRendered(int glyphIndex);

    fvPoint getUv(int glyphIndex);

    unsigned long getImage();

    unsigned long getCurrentAtlas(int* w, int* h);
};


#endif //FLAT_FLATFONTRENDER_H
