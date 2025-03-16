//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATCURVES_H
#define FLAT_FLATCURVES_H

#include "FlatVectorsBase.h"

class FlatCurves {
public:
    static void tessQuad(double scale,
                         double x1, double y1,
                         double x2, double y2,
                         double x3, double y3,
                         FlatVectors *context, void (*vertex)(FlatVectors *, double, double));

    static void tessCubic(double scale,
                          double x1, double y1,
                          double x2, double y2,
                          double x3, double y3,
                          double x4, double y4,
                          FlatVectors *context, void (*vertex)(FlatVectors *, double, double));
};


#endif //FLAT_FLATCURVES_H
