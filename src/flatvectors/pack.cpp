//
// Created by Rodrigo on 04/02/2025.
//

#include <cmath>
#include "pack.h"

int __findOpenCell(fvPack* pack, int w, int h) {
    int cellW = w <= pack->cellWidth ? 1 : (int)(ceil(w / (float)pack->cellWidth));
    int cellH = h <= pack->cellHeight ? 1 : (int)(ceil(h / (float)pack->cellHeight));
    bool single = cellW == 1 && cellH == 1;
    bool updateEmpty = false;

    int x = pack->minX;
    int y = pack->minY;
    int wc = pack->widthCount;
    int hc = pack->heightCount;
    int open = -1;
    for (; y + cellH <= hc; ++y) {
        int yOff = y * wc;
        for (; x + cellW <= wc;) {
            int index = yOff + x;
            fvCell rect = pack->matrix[index];
            if (rect.id == 0) {
                if (!updateEmpty) {
                    updateEmpty = true;
                    pack->minX = x;
                    pack->minY = y;
                }
                if (single) {
                    return index;
                }

                int jump = -1;
                for (int j = 0; j < cellH; ++j) {
                    int jOff = j * wc;
                    for (int i = 0; i < cellW; ++i) {
                        fvCell iRect = pack->matrix[index + jOff + i];
                        if (iRect.id != 0) {
                            jump = iRect.w;
                            break;
                        }
                    }
                    if (jump != -1) break;
                }

                if (jump == -1) {
                    return index;
                } else {
                    x += jump;
                }
            } else {
                x += rect.w;
            }
        }
        x = 0;
    }
    return open;
}

void __setCell(fvPack* pack, int w, int h, int openCell) {
    int wc = pack->widthCount;
    int cellW = w <= pack->cellWidth ? 1 : (int)(ceil(w / (float)pack->cellWidth));
    int cellH = h <= pack->cellHeight ? 1 : (int)(ceil(h / (float)pack->cellHeight));
    for (int y = 0; y < cellH; ++y) {
        int yOff = y * wc;
        for (int x = 0; x < cellW; ++x) {
            int index = yOff + x;
            pack->matrix[openCell + index].id = 1;
            pack->matrix[openCell + index].w = cellW;
            pack->matrix[openCell + index].h = cellH;
        }
    }
}

fvPack* packCreate(int cellWidth, int cellHeight) {
    fvPack* pack = (fvPack *) malloc(sizeof(fvPack));
    pack->cellWidth = cellWidth;
    pack->cellHeight = cellHeight;

    int idealWidth = cellWidth * 4;
    int idealHeight = cellHeight * 2;
    int power = 16;
    while (power < idealHeight || power * 2 < idealWidth) {
        power += power;
    }
    idealWidth = power * 2;
    idealHeight = power;

    pack->width = idealWidth;
    pack->height = idealHeight;
    pack->widthCount = idealWidth / cellWidth;
    pack->heightCount = idealHeight / cellHeight;
    pack->matrix = (fvCell *) calloc(pack->widthCount * pack->heightCount, sizeof(fvCell));
    pack->minX = 0;
    pack->minY = 0;

    return pack;
}

void packDestroy(fvPack* pack) {
    free(pack->matrix);
    free(pack);
}

bool packAddRect(fvPack* pack, int w, int h, int* x, int* y) {
    (*x) = -1;
    (*y) = -1;
    int openCell = __findOpenCell(pack, w, h);
    if (openCell > -1) {
        (*x) = openCell % pack->widthCount * pack->cellWidth;
        (*y) = openCell / pack->widthCount * pack->cellHeight;
        __setCell(pack, w, h, openCell);
        return false;

    } else if (packGrow(pack)) {
        openCell = __findOpenCell(pack, w, h);
        if (openCell > -1) {
            (*x) = openCell % pack->widthCount * pack->cellWidth;
            (*y) = openCell / pack->widthCount * pack->cellHeight;
            __setCell(pack, w, h, openCell);
            return true;

        } else { // Unusually bigger
            return true;
        }
    } else {
        // What to do now ?
        return false;
    }
}

bool packGrow(fvPack* pack) {
    int idealWidth;
    int idealHeight;
    if (pack->width <= pack->height) {
        idealWidth = pack->width * 2;
        idealHeight = pack->height;
    } else {
        idealWidth = pack->width;
        idealHeight = pack->height * 2;
    }

    if (idealWidth > 4096 || idealHeight > 4096) {
        return false;
    }

    int wc = pack->widthCount;
    int hc = pack->heightCount;
    int newWidthCount = idealWidth / pack->cellWidth;
    int newHeightCount = idealHeight / pack->cellHeight;

    fvCell * newMatrix = (fvCell *) calloc(newWidthCount * newHeightCount, sizeof(fvCell));
    for (int y = 0; y < hc; ++y) {
        for (int x = 0; x < wc; ++x) {
            newMatrix[y * newWidthCount + x] = pack->matrix[y * wc + x];
        }
    }
    free(pack->matrix);

    pack->matrix = newMatrix;
    pack->width = idealWidth;
    pack->height = idealHeight;
    pack->widthCount = newWidthCount;
    pack->heightCount = newHeightCount;
    pack->minX = 0;
    pack->minY = 0;
    return true;
}