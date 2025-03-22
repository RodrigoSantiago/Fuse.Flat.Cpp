//
// Created by Rodrigo on 15/03/2025.
//

#include <cmath>
#include "FlatPack.h"

FlatPack::FlatPack(int32 cellWidth, int32 cellHeight) {
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;

    int32 idealWidth = cellWidth * 4;
    int32 idealHeight = cellHeight * 2;
    int32 power = 16;
    while (power < idealHeight || power * 2 < idealWidth) {
        power += power;
    }
    idealWidth = power * 2;
    idealHeight = power;

    this->width = idealWidth;
    this->height = idealHeight;
    this->widthCount = idealWidth / cellWidth;
    this->heightCount = idealHeight / cellHeight;
    this->minX = 0;
    this->minY = 0;
    this->clearQuad = 0;

    this->matrix.resize(widthCount * heightCount);
}

FlatPack::~FlatPack() {

}

// Private

int32 FlatPack::findOpenCell(int32 cellW, int32 cellH) {
    bool single = cellW == 1 && cellH == 1;
    bool updateEmpty = false;

    int32 x = minX;
    int32 y = minY;
    int32 wc = widthCount;
    int32 hc = heightCount;
    int32 open = -1;
    for (; y + cellH <= hc; ++y) {
        int32 yOff = y * wc;
        for (; x + cellW <= wc;) {
            int32 index = yOff + x;
            fvCell rect = matrix[index];
            if (rect.uvPtr == nullptr) {
                if (!updateEmpty) {
                    updateEmpty = true;
                    minX = x;
                    minY = y;
                }
                if (single) {
                    return index;
                }

                int32 jump = -1;
                for (int32 j = 0; j < cellH; ++j) {
                    int32 jOff = j * wc;
                    for (int32 i = 0; i < cellW; ++i) {
                        fvCell iRect = matrix[index + jOff + i];
                        if (iRect.uvPtr != nullptr) {
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

void FlatPack::setCell(int32 cellW, int32 cellH, int32 openCell, fvPoint* point) {
    (*point).x = openCell % widthCount * cellWidth;
    (*point).y = openCell / widthCount * cellHeight;

    int32 wc = widthCount;
    for (int32 y = 0; y < cellH; ++y) {
        int32 yOff = y * wc;
        for (int32 x = 0; x < cellW; ++x) {
            int32 index = yOff + x;
            matrix[openCell + index].uvPtr = point;
            matrix[openCell + index].w = cellW;
            matrix[openCell + index].h = cellH;
        }
    }
}

// Public

int32 FlatPack::getWidth() {
    return width;
}

int32 FlatPack::getHeight() {
    return height;
}

void FlatPack::toCellSize(int32 w, int32 h, int32 *cellW, int32 *cellH) {
    *cellW = w <= cellWidth ? 1 : static_cast<int32>(ceil(w / static_cast<double>(cellWidth)));
    *cellH = h <= cellHeight ? 1 : static_cast<int32>(ceil(h / static_cast<double>(cellHeight)));
    *cellW *= cellWidth;
    *cellH *= cellHeight;
}

int32 FlatPack::addRect(int32 w, int32 h, fvPoint *point) {
    int32 cellW = w <= cellWidth ? 1 : static_cast<int32>(ceil(w / static_cast<double>(cellWidth)));
    int32 cellH = h <= cellHeight ? 1 : static_cast<int32>(ceil(h / static_cast<double>(cellHeight)));
    (*point).x = -1;
    (*point).y = -1;

    int32 openCell = findOpenCell(cellW, cellH);
    if (openCell > -1) {
        setCell(cellW, cellH, openCell, point);
        return 0;

    } else if (grow()) {
        openCell = findOpenCell(cellW, cellH);
        if (openCell > -1) {
            setCell(cellW, cellH, openCell, point);
        } else {
            return -1;
        }
        return 1;

    } else {
        clear();
        return 2;

    }
}

bool FlatPack::grow() {

    int32 idealWidth;
    int32 idealHeight;
    if (width <= height) {
        idealWidth = width * 2;
        idealHeight = height;
    } else {
        idealWidth = width;
        idealHeight = height * 2;
    }

    if (idealWidth > 4096 || idealHeight > 4096) {
        return false;
    }

    int32 wc = widthCount;
    int32 hc = heightCount;
    int32 newWidthCount = idealWidth / cellWidth;
    int32 newHeightCount = idealHeight / cellHeight;

    std::vector<fvCell> newVec = matrix;
    newVec.resize(newWidthCount * newHeightCount);
    for (int32 y = 0; y < hc; y++) {
        for (int32 x = 0; x < wc; x++) {
            newVec[y * newWidthCount + x] = matrix[y * wc + x];
        }
    }
    matrix = newVec;

    width = idealWidth;
    height = idealHeight;
    widthCount = newWidthCount;
    heightCount = newHeightCount;
    minX = 0;
    minY = 0;
    return true;
}

void FlatPack::clear() {
    int32 size = widthCount * heightCount;
    for (int32 i = 0; i < size; ++i) {
        if (matrix[i].uvPtr != nullptr) {
            (*matrix[i].uvPtr).x = -1;
            (*matrix[i].uvPtr).y = -1;
            matrix[i].uvPtr = nullptr;
        }
        matrix[i].w = 0;
        matrix[i].h = 0;
    }
    minX = 0;
    minY = 0;
}
