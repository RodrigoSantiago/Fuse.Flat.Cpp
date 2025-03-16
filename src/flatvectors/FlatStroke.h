//
// Created by Rodrigo on 16/03/2025.
//

#ifndef FLAT_FLATSTROKE_H
#define FLAT_FLATSTROKE_H

#include "FlatVectorsBase.h"

class FlatStroke {
public:
    float width;
    float miter;
    fvCap cap;
    fvJoin join;
    std::vector<float> dash;

    FlatStroke();
    FlatStroke(float width, float miter, fvCap cap, fvJoin join);
};


#endif //FLAT_FLATSTROKE_H
