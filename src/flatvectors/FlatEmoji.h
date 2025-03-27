//
// Created by Rodrigo on 25/03/2025.
//

#ifndef FLAT_FLATEMOJI_H
#define FLAT_FLATEMOJI_H

#include "FlatVectorsBase.h"
#include <glad/glad.h>
#include <unordered_map>

struct UnicodeCollection {
    uint32 arr[6];
    bool operator==(const UnicodeCollection& other) const {
        if (arr[0] != other.arr[0]) return false;
        if (arr[1] != other.arr[1]) return false;
        if (arr[2] != other.arr[2]) return false;
        if (arr[3] != other.arr[3]) return false;
        if (arr[4] != other.arr[4]) return false;
        if (arr[5] != other.arr[5]) return false;
        return true;
    }
};

namespace std {
    template <>
    struct hash<UnicodeCollection> {
        size_t operator()(const UnicodeCollection& ms) const {
            size_t hash_value = 0;
            hash_value ^= std::hash<uint32>{}(ms.arr[0]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<uint32>{}(ms.arr[1]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<uint32>{}(ms.arr[2]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<uint32>{}(ms.arr[3]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<uint32>{}(ms.arr[4]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            hash_value ^= std::hash<uint32>{}(ms.arr[5]) + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
            return hash_value;
        }
    };
}

class FlatEmoji {
    GLuint image;
    std::unordered_map<UnicodeCollection, fvPoint> unicodes;
public:
    FlatEmoji(GLuint image, int32* data, int32 length);

    ~FlatEmoji();

    static bool isEmoji(uint32 chr);

    static void readEmoji(const char *str, int32 strLen, int32 &i, uint32 &chr);

    GLuint getEmojiImage();

    fvPoint getEmojiUv(const char* str, int32 strLen, int32& i, uint32& chr);
};


#endif //FLAT_FLATEMOJI_H
