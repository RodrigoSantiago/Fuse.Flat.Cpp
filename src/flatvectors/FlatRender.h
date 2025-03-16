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
    int paint, vertex, element;

    bool debug;
    int curAA;
    GLuint curImage0;
    GLuint curImage1;

    GLuint shader;
    GLint viewID, matID, texID, fntID, stcID, dbgID;

public:
    FlatRender();
    ~FlatRender();

private:
    void ensureCapacity(int paint, int element, int vertex);

public:
    int renderAlign();

    // Render
    void begin(unsigned int width, unsigned int height, bool debug);
    void end();
    void clearClip(bool clip);
    void flush(fvPaint *paints, void* uniforms, int pSize,
               int* elements, int eSize,
               float *vtx, float *uvs, int vSize);

    // Font
    unsigned long createFontTexture(int width, int height);
    unsigned long resizeFontTexture(unsigned long oldImageID, int oldWidth, int oldHeight, int width, int height);
    void updateFontTexture(unsigned long imageID, void* data, int x, int y, int width, int height);
    void destroyFontTexture(unsigned long imageID);
};


#endif //FLAT_FLATRENDER_H
