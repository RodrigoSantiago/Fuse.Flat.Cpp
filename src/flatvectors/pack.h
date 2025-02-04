//
// Created by Rodrigo on 04/02/2025.
//

#ifndef FLAT_PACK_H
#define FLAT_PACK_H

#include "flatvectors.h"

fvPack* packCreate(int cellWidth, int cellHeight);

void packDestroy(fvPack* pack);

bool packAddRect(fvPack* pack, int w, int h, int* x, int* y);

bool packGrow(fvPack* pack);

#endif //FLAT_PACK_H
