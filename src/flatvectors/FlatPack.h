//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATPACK_H
#define FLAT_FLATPACK_H

#include <vector>
#include "FlatVectorsBase.h"

typedef struct fvCell {
    fvPoint* uvPtr;
    int32 w;
    int32 h;
} fvCell;

class FlatPack {
    int32 cellWidth;
    int32 cellHeight;
    int32 width;
    int32 height;
    int32 widthCount;
    int32 heightCount;
    int32 minX;
    int32 minY;
    int32 clearQuad;
    std::vector<fvCell> matrix;

public:
    FlatPack(int32 cellWidth, int32 cellHeight);

    ~FlatPack();
private:
    int32 findOpenCell(int32 cellW, int32 cellH);

    void setCell(int32 cellW, int32 cellH, int32 openCell, fvPoint* point);

public:
    int32 getWidth();

    int32 getHeight();

    void toCellSize(int32 w, int32 h, int32* cellW, int32* cellH);

    int32 addRect(int32 w, int32 h, fvPoint* point);

    bool grow();

    void clear();
};


#endif //FLAT_FLATPACK_H
