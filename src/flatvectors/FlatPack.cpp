//
// Created by Rodrigo on 15/03/2025.
//

#include <cmath>
#include "FlatPack.h"

FlatPack::FlatPack(int cellWidth, int cellHeight) {
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;

    int idealWidth = cellWidth * 4;
    int idealHeight = cellHeight * 2;
    int power = 16;
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

int FlatPack::findOpenCell(int cellW, int cellH) {
    bool single = cellW == 1 && cellH == 1;
    bool updateEmpty = false;

    int x = minX;
    int y = minY;
    int wc = widthCount;
    int hc = heightCount;
    int open = -1;
    for (; y + cellH <= hc; ++y) {
        int yOff = y * wc;
        for (; x + cellW <= wc;) {
            int index = yOff + x;
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

                int jump = -1;
                for (int j = 0; j < cellH; ++j) {
                    int jOff = j * wc;
                    for (int i = 0; i < cellW; ++i) {
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

void FlatPack::setCell(int cellW, int cellH, int openCell, fvPoint* point) {
    (*point).x = openCell % widthCount * cellWidth;
    (*point).y = openCell / widthCount * cellHeight;

    int wc = widthCount;
    for (int y = 0; y < cellH; ++y) {
        int yOff = y * wc;
        for (int x = 0; x < cellW; ++x) {
            int index = yOff + x;
            matrix[openCell + index].uvPtr = point;
            matrix[openCell + index].w = cellW;
            matrix[openCell + index].h = cellH;
        }
    }
}

// Public

int FlatPack::getWidth() {
    return width;
}

int FlatPack::getHeight() {
    return height;
}

void FlatPack::toCellSize(int w, int h, int *cellW, int *cellH) {
    *cellW = w <= cellWidth ? 1 : static_cast<int>(ceil(w / static_cast<double>(cellWidth)));
    *cellH = h <= cellHeight ? 1 : static_cast<int>(ceil(h / static_cast<double>(cellHeight)));
    *cellW *= cellWidth;
    *cellH *= cellHeight;
}

int FlatPack::addRect(int w, int h, fvPoint *point) {
    int cellW = w <= cellWidth ? 1 : static_cast<int>(ceil(w / static_cast<double>(cellWidth)));
    int cellH = h <= cellHeight ? 1 : static_cast<int>(ceil(h / static_cast<double>(cellHeight)));
    (*point).x = -1;
    (*point).y = -1;

    int openCell = findOpenCell(cellW, cellH);
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

    int idealWidth;
    int idealHeight;
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

    int wc = widthCount;
    int hc = heightCount;
    int newWidthCount = idealWidth / cellWidth;
    int newHeightCount = idealHeight / cellHeight;

    matrix.resize(newWidthCount * newHeightCount);
    for (int y = hc - 1; y >= 0; --y) {
        for (int x = wc - 1; x >= 0; --x) {
            matrix[y * newWidthCount + x] = matrix[y * wc + x];
        }
    }

    width = idealWidth;
    height = idealHeight;
    widthCount = newWidthCount;
    heightCount = newHeightCount;
    minX = 0;
    minY = 0;
    return true;
}

void FlatPack::clear() {
    int size = widthCount * heightCount;
    for (int i = 0; i < size; ++i) {
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
