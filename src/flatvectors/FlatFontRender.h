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
    uint32 imageID;

public:
    FlatFontRender(FlatFont* font, FlatRender* render);

    ~FlatFontRender();

    FlatFont* getFont();

    int32 renderGlyph(fvGlyph& glyph, int32 glyphIndex);

    bool isGlyphRendered(int32 glyphIndex);

    fvPoint getUv(int32 glyphIndex);

    uint32 getImage();

    uint32 getCurrentAtlas(int32* w, int32* h);
};


#endif //FLAT_FLATFONTRENDER_H
