//
// Created by Rodrigo on 14/03/2025.
//

#ifndef FLAT_FLATRENDER_H
#define FLAT_FLATRENDER_H

#include "FlatVectorsBase.h"
#include <glad/glad.h>

class FlatRender {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint ubo;
    int32 paint, vertex, element;

    bool debug;
    int32 curAA;
    fvBlendMode curBm;
    GLuint curImage0;
    GLuint curImage1;
    GLuint curImage2;

    GLuint shader;
    GLint viewID, matID, texID, fntID, emjID, stcID, dbgID;

    int32 maxTextureSize;
    int32 maxUniforms;
    int32 maxElements;
    int32 maxVertices;

public:
    FlatRender();
    ~FlatRender();

private:
    void ensureCapacity(int32 paint, int32 element, int32 vertex);

public:
    int32 renderAlign();
    int32 getMaxTextureSize();
    int32 getMaxUniforms();
    int32 getMaxElements();
    int32 getMaxVertices();

    // Render
    void begin(unsigned int32 width, unsigned int32 height, bool debug);
    void end();
    void clearClip(bool clip);
    void flush(fvPaint *paints, void* uniforms, int32 pSize,
               int32* elements, int32 eSize,
               float *vtx, float *uvs, int32 vSize);

    // Font
    uint32 createFontTexture(int32 width, int32 height);
    uint32 resizeFontTexture(uint32 oldImageID, int32 oldWidth, int32 oldHeight, int32 width, int32 height);
    void updateFontTexture(uint32 imageID, void* data, int32 x, int32 y, int32 width, int32 height);
    void destroyFontTexture(uint32 imageID);
};


#endif //FLAT_FLATRENDER_H
