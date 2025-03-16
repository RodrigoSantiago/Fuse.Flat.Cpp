//
// Created by Rodrigo on 16/03/2025.
//

#include "FlatStroke.h"

FlatStroke::FlatStroke() {
    width = 0;
    miter = 0;
    cap = fvCap::CAP_BUTT;
    join = fvJoin::JOIN_BEVEL;
}

FlatStroke::FlatStroke(float width, float miter, fvCap cap, fvJoin join) :
    width(width), miter(miter), cap(cap), join(join) {

}
