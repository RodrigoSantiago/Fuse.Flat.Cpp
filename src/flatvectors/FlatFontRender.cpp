//
// Created by Rodrigo on 15/03/2025.
//

#include <cmath>
#include "FlatFontRender.h"
#include "FlatFont.h"
#include "FlatRender.h"
#include "FlatPack.h"

FlatFontRender::FlatFontRender(FlatFont *font, FlatRender *render) : imageID(0) {
    this->font = font;
    this->render = render;

    int len = font->getGlyphCount();
    renderState = new fvPoint [len];
    for (int i = 0; i < len; i++) {
        renderState[i] = {-1, -1};
    }

    pack = new FlatPack(font->getCellW(), font->getCellH());
}

FlatFontRender::~FlatFontRender() {
    render->destroyFontTexture(imageID);
    delete[] renderState;
    delete pack;
}

FlatFont* FlatFontRender::getFont() {
    return font;
}

int FlatFontRender::renderGlyph(fvGlyph& glyph, int glyphIndex) {
    int width = (int) ceil(glyph.w);
    int height = (int) ceil(glyph.h);

    if (width > 0 && height > 0) {
        int oW = pack->getWidth();
        int oH = pack->getHeight();

        fvPoint * point = &renderState[glyphIndex];
        int state = pack->addRect(width, height, point);

        if (state == 2) {
            return 2; // PACK CLEARED - NO TEXTURE CREATED/MODIFIED

        } else if (state != -1) {
            if (imageID == 0) {
                imageID = render->createFontTexture(pack->getWidth(), pack->getHeight());
            } else if (state == 1) {
                imageID = render->resizeFontTexture(imageID, oW, oH, pack->getWidth(), pack->getHeight());
            }

            int cellW, cellH;
            pack->toCellSize(width, height, &cellW, &cellH);

            render->updateFontTexture(imageID, glyph.cell, point->x, point->y, cellW, cellH);
            return 1; // TEXTURE CREATED/MODIFIED
        }
    }

    renderState[glyphIndex] = fvPoint {-2, -2};
    return 0; // NOTHING HAPPENED, NO TEXTURE CREATED/MODIFIED
}

bool FlatFontRender::isGlyphRendered(int glyphIndex) {

    return renderState[glyphIndex].x != -1;
}


fvPoint FlatFontRender::getUv(int glyphIndex) {

    return renderState[glyphIndex];
}

unsigned long FlatFontRender::getImage() {

    return imageID;
}

unsigned long FlatFontRender::getCurrentAtlas(int* w, int* h) {
    *w = pack->getWidth();
    *h = pack->getHeight();
    return imageID;
}