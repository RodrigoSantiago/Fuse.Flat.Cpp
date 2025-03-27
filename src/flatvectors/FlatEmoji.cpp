//
// Created by Rodrigo on 25/03/2025.
//

#include "FlatEmoji.h"
#include "FlatText.h"
#include "FlatVectors.h"

FlatEmoji::FlatEmoji(GLuint image, int32 *data, int32 length) : image(image) {
    // 64 * 64 = 4096 max icons
    const float w = 64.0 / 4096.0;

    for (int32 i = 0; i < length; i+= 6) {
        UnicodeCollection sca = {{
            (uint32) data[i]    , (uint32) data[i + 1], (uint32) data[i + 2],
            (uint32) data[i + 3], (uint32) data[i + 4], (uint32) data[i + 5]}};
        unicodes[sca] = {
                (i / 6) % 64 * w, (i / 6) / 64 * w
        };
    }
}

FlatEmoji::~FlatEmoji() {

}

bool FlatEmoji::isEmoji(uint32 chr) {
    return FlatVectors::isEmojiRender() &&
           ((chr >= 0x1F000 && chr <= 0x1FAFF) || (chr >= 0x200D && chr <= 0x3300) || chr == 0xFE0E || chr == 0xFE0F);
}

void FlatEmoji::readEmoji(const char *str, int32 strLen, int32 &i, uint32 &chr) {
    int32 pi = i;
    uint32 pc = chr;

    bool waitNext = false;
    int32 pos = 1;
    while (FlatText::utf8loop(str, strLen, i, chr)) {
        if (chr == 0x200D) {
            waitNext = true;
        } else if (waitNext || chr == 0x1F3FB || chr == 0x1F3FC || chr == 0x1F3FD || chr == 0x1F3FE || chr == 0x1F3FF) {
            pos++;
            waitNext = false;
            if (pos == 6) {
                break;
            }
        } else if (pos == 1 && (pc >= 0x1F1E6 && pc <= 0x1F1FF) && (chr >= 0x1F1E6 && chr <= 0x1F1FF)) { // FLAGS
            break;
        } else if (chr != 0xFE0E && chr != 0xFE0F) {
            i = pi;
            chr = pc;
            break;
        }
        pi = i;
        pc = chr;
    }
}

GLuint FlatEmoji::getEmojiImage() {
    return image;
}

fvPoint FlatEmoji::getEmojiUv(const char *str, int32 strLen, int32 &i, uint32 &chr) {
    int32 pi = i;
    uint32 pc = chr;

    bool waitNext = false;
    int32 pos = 1;
    UnicodeCollection sca = {{chr, 0, 0, 0, 0, 0}};
    while (FlatText::utf8loop(str, strLen, i, chr)) {
        if (chr == 0x200D) {
            waitNext = true;
        } else if (waitNext || chr == 0x1F3FB || chr == 0x1F3FC || chr == 0x1F3FD || chr == 0x1F3FE || chr == 0x1F3FF) {
            sca.arr[pos++] = chr;
            waitNext = false;
            if (pos == 6) {
                break;
            }
        } else if (pos == 1 && (pc >= 0x1F1E6 && pc <= 0x1F1FF) && (chr >= 0x1F1E6 && chr <= 0x1F1FF)) { // FLAGS
            sca.arr[pos++] = chr;
            break;
        } else if (chr != 0xFE0E && chr != 0xFE0F) {
            i = pi;
            chr = pc;
            break;
        }
        pi = i;
        pc = chr;
    }

    auto it = unicodes.find(sca);
    if (it != unicodes.end()) {
        return it->second;
    } else {
        return {0, 0};
    }
}