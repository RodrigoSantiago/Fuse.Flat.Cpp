//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATTEXT_H
#define FLAT_FLATTEXT_H

#include "FlatVectorsBase.h"

class FlatText {
private:
    static int32 readChar(const char* str, int32 strLen, int32& i, uint32& unicode);

public:
    static int32 utf8loop(const char* str, int32 strLen, int32& i, uint32& out);
};


#endif //FLAT_FLATTEXT_H
