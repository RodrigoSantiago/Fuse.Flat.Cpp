//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATPACK_H
#define FLAT_FLATPACK_H

#include <vector>
#include "FlatVectorsBase.h"

typedef struct fvCell {
    fvPoint* uvPtr;
    int w;
    int h;
} fvCell;

class FlatPack {
    int cellWidth;
    int cellHeight;
    int width;
    int height;
    int widthCount;
    int heightCount;
    int minX;
    int minY;
    int clearQuad;
    std::vector<fvCell> matrix;

public:
    FlatPack(int cellWidth, int cellHeight);

    ~FlatPack();
private:
    int findOpenCell(int cellW, int cellH);

    void setCell(int cellW, int cellH, int openCell, fvPoint* point);

public:
    int getWidth();

    int getHeight();

    void toCellSize(int w, int h, int* cellW, int* cellH);

    int addRect(int w, int h, fvPoint* point);

    bool grow();

    void clear();
};


#endif //FLAT_FLATPACK_H
