//
// Created by Rodrigo on 15/03/2025.
//

#include "FlatText.h"

int32 FlatText::readChar(const char* str, int32 strLen, int32& i, uint32& unicode) {
    if (i >= strLen) return 0;

    if (((unsigned char) str[i] | 0b01111111u) == 0b01111111u) {    //0xxxxxxx
        unicode = ((uint32) str[i]) & 127u;
        i += 1;
    } else if (((unsigned char) str[i] >> 5) == 0b110u) {           //110xxxxx
        if (i + 1 < strLen) {
            unicode = (((uint32) str[i]) & 31u) << 6u |
                      (((uint32) str[i + 1]) & 63u);
            i += 2;
        } else {
            return 0;
        }
    } else if (((unsigned char) str[i] >> 4) == 0b1110u) {          //1110xxxx
        if (i + 2 < strLen) {
            unicode = (((uint32) str[i]) & 15u) << 12u |
                      (((uint32) str[i + 1]) & 63u) << 6u |
                      (((uint32) str[i + 2]) & 63u);
            i += 3;
        } else {
            return 0;
        }
    } else if (((unsigned char) str[i] >> 3) == 0b11110u) {         //11110xxx
        if (i + 3 < strLen) {
            unicode = (((uint32) str[i]) & 7u) << 18u |
                      (((uint32) str[i + 1]) & 63u) << 12u |
                      (((uint32) str[i + 2]) & 63u) << 6u |
                      (((uint32) str[i + 3]) & 63u);
            i += 4;
        } else {
            return 0;
        }
    } else {                            //invalid character
        unicode = 0xFFFD;
        i += 1;
    }
    return 1;
}

int32 FlatText::utf8loop(const char* str, int32 strLen, int32& i, uint32& out) {
    if (readChar(str, strLen, i, out)) {
        if (out >= 0xD800 && out <= 0xDBFF) {
            uint32 high = out;
            if (readChar(str, strLen, i, out)) {
                if (out >= 0xDC00 && out <= 0xDFFF) {
                    out = (((high - 0xD800) << 10) | (out - 0xDC00)) + 0x10000;
                } else {
                    out = 0xFFFD;
                }
                return 1;
            } else {
                return 0;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}