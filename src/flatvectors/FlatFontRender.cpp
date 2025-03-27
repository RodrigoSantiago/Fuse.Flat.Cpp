//
// Created by Rodrigo on 15/03/2025.
//

#include <cmath>
#include <iostream>
#include "FlatFontRender.h"
#include "FlatFont.h"
#include "FlatRender.h"
#include "FlatPack.h"

FlatFontRender::FlatFontRender(FlatFont *font, FlatRender *render) : imageID(0) {
    this->font = font;
    this->render = render;

    int32 len = font->getGlyphCount();
    renderState = new fvPoint [len];
    for (int32 i = 0; i < len; i++) {
        renderState[i] = {-1, -1};
    }

    pack = new FlatPack(font->getCellW(), font->getCellH(), render->getMaxTextureSize(), render->getMaxTextureSize());
}

FlatFontRender::~FlatFontRender() {
    render->destroyFontTexture(imageID);
    delete[] renderState;
    delete pack;
}

FlatFont* FlatFontRender::getFont() {
    return font;
}

int32 FlatFontRender::renderGlyph(fvGlyph& glyph, int32 glyphIndex) {
    int32 width = (int32) ceil(glyph.w);
    int32 height = (int32) ceil(glyph.h);

    if (width > 0 && height > 0) {
        int32 oW = pack->getWidth();
        int32 oH = pack->getHeight();

        fvPoint * point = &renderState[glyphIndex];
        int32 state = pack->addRect(width, height, point);

        if (state == 2) {
            return 2; // PACK CLEARED - NO TEXTURE CREATED/MODIFIED

        } else if (state != -1) {
            if (imageID == 0) {
                imageID = render->createFontTexture(pack->getWidth(), pack->getHeight());
            } else if (state == 1) {
                imageID = render->resizeFontTexture(imageID, oW, oH, pack->getWidth(), pack->getHeight());
            }

            int32 cellW, cellH;
            pack->toCellSize(width, height, &cellW, &cellH);

            render->updateFontTexture(imageID, glyph.cell, point->x, point->y, cellW, cellH);
            return 1; // TEXTURE CREATED/MODIFIED
        }
    }

    renderState[glyphIndex] = fvPoint {-2, -2};
    return 0; // NOTHING HAPPENED, NO TEXTURE CREATED/MODIFIED
}

bool FlatFontRender::isGlyphRendered(int32 glyphIndex) {

    return renderState[glyphIndex].x != -1;
}


fvPoint FlatFontRender::getUv(int32 glyphIndex) {

    return renderState[glyphIndex];
}

uint32 FlatFontRender::getImage() {

    return imageID;
}

uint32 FlatFontRender::getCurrentAtlas(int32* w, int32* h) {
    *w = pack->getWidth();
    *h = pack->getHeight();
    return imageID;
}