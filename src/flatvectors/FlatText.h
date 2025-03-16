//
// Created by Rodrigo on 15/03/2025.
//

#ifndef FLAT_FLATTEXT_H
#define FLAT_FLATTEXT_H

#include "FlatVectorsBase.h"

class FlatText {
private:
    static int readChar(const char* str, int strLen, int& i, unsigned long& unicode);

public:
    static int utf8loop(const char* str, int strLen, int& i, unsigned long& out);
};


#endif //FLAT_FLATTEXT_H
