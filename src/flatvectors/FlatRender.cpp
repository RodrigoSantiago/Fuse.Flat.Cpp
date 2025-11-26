//
// Created by Rodrigo on 14/03/2025.
//

#include "FlatRender.h"
#include "FlatFont.h"
#include "FlatFontRender.h"
#include "FlatVectors.h"
#include "FlatEmoji.h"
#include <iostream>
#include <cmath>

const char *vertexSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 iPos;\n"
        "layout (location = 1) in vec2 iTex;\n"
        "uniform vec2 view;\n"
        "uniform mat3 mat;\n"
        "out vec2 oPos;\n"
        "out vec2 oTex;\n"
        "out float oEmj;\n"
        "void main() {\n"
        "   oPos = iPos;\n"
        "   oTex = iTex.x < 0 ? vec2(2 + iTex.x, 2 + iTex.y) : iTex;\n"
        "   oEmj = iTex.x < 0 ? 1 : 0;\n"
        "   vec2 pos;\n"
        "   pos.x = iPos.x * mat[0][0] + iPos.y * mat[0][2] + mat[1][1];\n"
        "   pos.y = iPos.x * mat[0][1] + iPos.y * mat[1][0] + mat[1][2];\n"
        "	gl_Position = vec4(pos.x * 2.0 / view.x - 1.0, 1.0 - pos.y * 2.0 / view.y, 0, 1);\n"
        "}\0";

const char *fragmentSource =
        "#version 330 core\n"
        "out vec4 FragColor;"
        "layout (std140) uniform Paint {"
        "    vec4 data;"
        "    mat3 colorMat;"
        "    vec4 shape;"
        "    vec4 extra;"
        "    vec4 stops[4];"
        "    vec4 colors[16];"
        "};"
        "uniform int stc;"
        "uniform int dbg;"
        "uniform sampler2D tex;"
        "uniform sampler2D fnt;"
        "uniform sampler2D emj;"
        "in vec2 oPos;"
        "in vec2 oTex;"
        "in float oEmj;"
        "float roundrect(vec2 pt, vec2 ext, float rad) {"
        "    vec2 ext2 = ext - vec2(rad);"
        "    vec2 d = abs(pt) - ext2;"
        "    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;"
        "}"
        "float focuscircle(vec2 coord, vec2 focus) {"
        "    vec2 diff = focus;"
        "    float len = length(coord - focus);"
        "    vec2 rayDir = (coord - focus) / len;"
        "    float b = 2.0 * dot(rayDir, diff);"
        "    float c = dot(diff, diff) - 1.0;"
        "    float disc = b * b - 4.0 * dot(rayDir, rayDir) * c;"
        "    float gradLength = 1.0;"
        "    if (disc >= 0.0) {"
        "        float t = (-b + sqrt(disc)) * 0.5;"
        "        vec2 projection = focus + rayDir * t;"
        "        gradLength = distance(projection, focus);"
        "    }"
        "    return len / gradLength;"
        "}"
        "float expin(float a) {"
        "    return 1.0 - (1.0 - a) * (1.0 - a);"
        "}"
        "vec4 gradientColor(float t) {"
        "    for (int i = 0; i < 16; i++) {"
        "        int stopIndex = i / 4;"
        "        int compIndex = i % 4;"
        "        float start = stops[stopIndex][compIndex];"
        "        float end = stops[stopIndex][compIndex + 1];"
        "        if (t >= start && t <= end) {"
        "            float localT = clamp((t - start) / (end - start), 0.0, 1.0);"
        "            return mix(colors[i], colors[i + 1], localT);"
        "        }"
        "    }"
        "    return colors[0];"
        "}"
        "void main() {"
        "    vec4 frag;"
        "    if (stc == 1) {"
        "        frag = vec4(1.0);"
        "    } else if (int(data[0]) == 0 || dbg == 1) {"
        "        frag = colors[0];"
        "    } else if (int(data[0]) == 1) {"
        "        vec2 cPt = (colorMat * vec3(oPos, 1.0)).xy;"
        "        float a = 1;"
        "        float t;"
        "        if (extra[2] == 0.0)"
        "            t = (roundrect(cPt, shape.xy, shape.z) + shape.w * 0.5) / shape.w;"
        "        else"
        "            t = focuscircle(cPt / (shape.z * 2.0), extra.xy);"
        "        int wrapMode = int(data[2]);"
        "        if (wrapMode == 0) t = clamp(t, 0.0, 1.0);"
        "        else if (wrapMode == 1) t = fract(t);"
        "        else if (wrapMode == 2) t = mod(floor(t), 2.0) == 0.0 ? fract(t) : 1.0 - fract(t);"
        "        else if (wrapMode == 3 && (t < 0.0 || t > 1.0)) {"
        "           a = 0;"
        "        }"
        "        frag = gradientColor(t) * a;"
        "    } else if (int(data[0]) == 2) {"
        "        vec2 cPt = (colorMat * vec3(oPos, 1.0)).xy;"
        "        float t = (roundrect(cPt, shape.xy, shape.z) + shape.w * 0.5) / shape.w;"
        "        t = expin(clamp(t, 0.0, 1.0));"
        "        frag = mix(colors[0], colors[1], t);"
        "    } else if (int(data[0]) == 3) {"
        "        vec2 tPt = (colorMat * vec3(oPos, 1.0)).xy;"
        "        ivec2 sz = textureSize(tex, 0);"
        "        float a = 1;"
        "        int wrapMode = int(data[2]);"
        "        if (wrapMode == 0) tPt = clamp(tPt, 0.0, 1.0);"
        "        else if (wrapMode == 1) tPt = fract(tPt);"
        "        else if (wrapMode == 2) {"
        "            tPt = vec2("
        "                mod(floor(tPt.x), 2.0) == 0.0 ? fract(tPt.x) : 1.0 - fract(tPt.x),"
        "                mod(floor(tPt.y), 2.0) == 0.0 ? fract(tPt.y) : 1.0 - fract(tPt.y)"
        "            );"
        "        } else if (wrapMode == 3 && (tPt.x < 0.0 || tPt.x > 1.0 || tPt.y < 0.0 || tPt.y > 1.0)) {"
        "           a = 0;"
        "        }"
        "        if (int(data[1]) == 1) tPt = (round(tPt * sz - 0.5) + 0.5) / vec2(sz);"
        "        frag = colors[0] * texture(tex, tPt) * a;"
        "    } else if (int(data[0]) == 4 && oEmj > 0.0) {"
        "        vec4 emjCol = texture(emj, oTex);"
        "        emjCol.a *= colors[0].a;"
        "        frag = emjCol;"
        "    } else if (int(data[0]) == 4) {"
        "        ivec2 sz = textureSize(fnt, 0);"
        "        float a = texture(fnt, oTex / vec2(sz)).r;"
        "        if (extra[3] >= 0.0) {"
        "            float screenSpaceScale = fwidth(oTex).x * 0.05 + extra[3] * 0.5;"
        "            a = smoothstep(0.5 - screenSpaceScale, 0.5 + screenSpaceScale, a);"
        "        }"
        "        frag = colors[0];"
        "        frag.a *= a;"
        "    } else {"
        "        frag = vec4(1.0);"
        "    }"
        "    FragColor = vec4(frag.rgb * frag.a, frag.a);"
        "}";

// Local Private

int32 _get_align() {
    GLint align;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);
    return (GLint) ((sizeof(fvUniform) - 1) / align + 1) * align;
}

void _render_triangles(int32 pos, int32 length) {
    glDrawElements(GL_TRIANGLES, (GLsizei) (length), GL_UNSIGNED_INT, (void*) (pos * sizeof(int32)));
}

void _setBlendMode(fvBlendMode blendMode) {
    if (blendMode == SUB) {
        glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
    } else if (blendMode == REV_SUB) {
        glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD);
    } else if (blendMode == DARKEN) {
        glBlendEquationSeparate(GL_MIN, GL_FUNC_ADD);
    } else if (blendMode == LIGHTEN) {
        glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
    } else {
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    }
    switch (blendMode) {
        case SRC_OVER:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case DST_OVER:
            glBlendFuncSeparate(
                    GL_ONE_MINUS_DST_ALPHA, GL_ONE,
                    GL_ONE_MINUS_DST_ALPHA, GL_ONE);
            break;
        case SRC_IN:
            glBlendFuncSeparate(
                    GL_DST_ALPHA, GL_ZERO,
                    GL_DST_ALPHA, GL_ZERO);
            break;
        case DST_IN:
            glBlendFuncSeparate(
                    GL_ZERO, GL_SRC_ALPHA,
                    GL_ZERO, GL_SRC_ALPHA);
            break;
        case SRC_OUT:
            glBlendFuncSeparate(
                    GL_ONE_MINUS_DST_ALPHA, GL_ZERO,
                    GL_ONE_MINUS_DST_ALPHA, GL_ZERO);
            break;
        case DST_OUT:
            glBlendFuncSeparate(
                    GL_ZERO, GL_ONE_MINUS_SRC_ALPHA,
                    GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case SRC_ATOP:
            glBlendFuncSeparate(
                    GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                    GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case DST_ATOP:
            glBlendFuncSeparate(
                    GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA,
                    GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA);
            break;
        case XOR:
            glBlendFuncSeparate(
                    GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                    GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case CLEAR:
            glBlendFuncSeparate(
                    GL_ZERO, GL_ZERO,
                    GL_ZERO, GL_ZERO);
            break;
        case SRC:
            glBlendFuncSeparate(
                    GL_ONE, GL_ZERO,
                    GL_ONE, GL_ZERO);
            break;
        case DST:
            glBlendFuncSeparate(
                    GL_ZERO, GL_ONE,
                    GL_ZERO, GL_ONE);
            break;
        case ADD:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case SUB:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case MUL:
            glBlendFuncSeparate(
                    GL_DST_COLOR, GL_ZERO,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case DARKEN:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case LIGHTEN:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case REV_SUB:
            glBlendFuncSeparate(
                    GL_ONE, GL_ONE,
                    GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            break;
    }
}

// Class

FlatRender::FlatRender() : paint(0), vertex(0), element(0), curAA(0), curImage0(0), curImage1(0) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenBuffers(1, &ubo);

    // Vertex Shader
    int success;
    char infoLog[512];
    GLuint vS = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vS, 1, &vertexSource, NULL);
    glCompileShader(vS);
    glGetShaderiv(vS, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vS, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    GLuint fS = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fS, 1, &fragmentSource, NULL);
    glCompileShader(fS);
    glGetShaderiv(fS, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fS, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    shader = glCreateProgram();
    glAttachShader(shader, vS);
    glAttachShader(shader, fS);
    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vS);
    glDeleteShader(fS);

    GLuint paintIndex = glGetUniformBlockIndex(shader, "Paint");
    glUniformBlockBinding(shader, paintIndex, 0);
    viewID = glGetUniformLocation(shader, "view");
    matID = glGetUniformLocation(shader, "mat");
    texID = glGetUniformLocation(shader, "tex");
    fntID = glGetUniformLocation(shader, "fnt");
    emjID = glGetUniformLocation(shader, "emj");
    stcID = glGetUniformLocation(shader, "stc");
    dbgID = glGetUniformLocation(shader, "dbg");

    GLint maxUniformBlockSize;
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    GLint maxEboSize;
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxEboSize);
    GLint maxVboSize;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxVboSize);
    GLint maxTexSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTexSize);

    maxUniforms = std::min((int32)512, (int32)(maxUniformBlockSize / renderAlign()));
    maxElements = std::min((int32)32768, (int32)(maxEboSize));
    maxVertices = std::min((int32)32768, (int32)(maxVboSize));
    maxTextureSize = std::min((int32)4096, (int32)(maxTexSize));
}

FlatRender::~FlatRender() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &ubo);
    glDeleteShader(shader);
}

int32 FlatRender::renderAlign() {
    static int32 align = _get_align();
    return align;
}

int32 FlatRender::getMaxTextureSize() {
    return maxTextureSize;
}

int32 FlatRender::getMaxUniforms() {
    return maxUniforms;
}

int32 FlatRender::getMaxElements() {
    return maxElements;
}

int32 FlatRender::getMaxVertices() {
    return maxVertices;
}

void FlatRender::ensureCapacity(int32 paint, int32 element, int32 vertex) {

    // Uniform Buffer
    if (this->paint < paint) {
        this->paint = paint;
        glBufferData(GL_UNIFORM_BUFFER, paint * renderAlign(), NULL, GL_STATIC_DRAW);
    }

    // Vertices + UVs
    if (this->vertex < vertex) {
        this->vertex = vertex;
        glBufferData(GL_ARRAY_BUFFER, vertex * sizeof(float) * 2, NULL, GL_STATIC_DRAW);

        // pos
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        // uv
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) (vertex * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // Elements
    if (this->element < element) {
        this->element = element;

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, element * sizeof(int32), NULL, GL_STATIC_DRAW);
    }
}

void FlatRender::begin(unsigned int32 width, unsigned int32 height, bool dbg) {
    curAA = 0;
    curImage0 = 0;
    curImage1 = 0;
    debug = dbg;
    curBm = fvBlendMode::SRC_OVER;

    glUseProgram(shader);
    glUniform2f(viewID, width, height);
    glUniform1i(texID, 0);
    glUniform1i(fntID, 1);
    glUniform1i(emjID, 2);
    glUniform1i(stcID, 0);
    glUniform1i(dbgID, debug ? 1 : 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindBuffer(GL_UNIFORM_BUFFER, ubo);

    glDisable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0x80, 0xFF);
    glStencilMask(0xFF);

    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void FlatRender::end() {
    glUseProgram(0);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    if (curImage1 != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (curImage2 != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FlatRender::clearClip(bool clip) {
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glClearStencil(clip ? 0x80 : 0x00);
    glClear(GL_STENCIL_BUFFER_BIT);

    glStencilFunc(GL_EQUAL, 0x00, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void FlatRender::flush(
        fvPaint *paints, void* uniforms, int32 pSize,
        int32* elements, int32 eSize,
        float *vtx, float *uvs, int32 vSize) {

    ensureCapacity(pSize, eSize, vSize);

    glBufferSubData(GL_UNIFORM_BUFFER, 0, pSize * renderAlign(), uniforms);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, eSize * sizeof(int32), elements);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vSize * sizeof(float), vtx);
    glBufferSubData(GL_ARRAY_BUFFER, vertex * sizeof(float), vSize * sizeof(float), uvs);

    GLsizei pos = 0;
    for (int32 i = 0; i < pSize; i++) {
        fvPaint &curPaint = paints[i];
        int32 totalElements = curPaint.elements * 3;
        int32 renderElements = totalElements - pos;

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, i * renderAlign(), renderAlign());
        glUniformMatrix3fv(matID, 1, 0, curPaint.transform);

        // Antialiasing
        if (curAA != curPaint.antiAlias) {
            curAA = curPaint.antiAlias;
            if (curAA) {
                glEnable(GL_MULTISAMPLE);
            } else {
                glDisable(GL_MULTISAMPLE);
            }
        }

        if (curPaint.pathOp == CLIP || curPaint.pathOp == UNCLIP) {
            if (curBm != fvBlendMode::SRC_OVER) {
                curBm = fvBlendMode::SRC_OVER;
                _setBlendMode(curBm);
            }
            glColorMask(0, 0, 0, 0);
            glUniform1i(stcID, 1);

            if (curPaint.convex) {
                unsigned char val = curPaint.pathOp == CLIP ? 0x80 : 0x00;

                glStencilFunc(GL_ALWAYS, val, 0xFF);
                glStencilMask(0xFF);
                glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
                _render_triangles(pos, renderElements);
            } else {
                glStencilFunc(GL_ALWAYS, 0x01, 0xFF);
                glStencilMask(0x01);
                glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                _render_triangles(pos, renderElements);

                if (curPaint.pathOp == UNCLIP) {
                    glStencilFunc(GL_EQUAL, 0x01, 0x01);
                    glStencilMask(0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
                    _render_triangles(pos, renderElements);
                } else {
                    glStencilFunc(GL_EQUAL, 0x81, 0x01);
                    glStencilMask(0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                    _render_triangles(pos, renderElements);

                    glStencilFunc(GL_EQUAL, 0x01, 0x01);
                    glStencilMask(0x01);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
                    _render_triangles(pos, renderElements);
                }
            }

            glStencilFunc(GL_EQUAL, 0x00, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilMask(0xFF);
            glColorMask(1, 1, 1, 1);
            glUniform1i(stcID, 0);
        } else {
            if (curBm != curPaint.blendMode) {
                curBm = curPaint.blendMode;
                _setBlendMode(curBm);
            }

            if (curImage0 != curPaint.image0) {
                curImage0 = curPaint.image0;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, curImage0);
            }

            GLuint fntImg = curPaint.pathOp == TEXT && curPaint.font != nullptr ?
                    curPaint.font->getImage() : 0;

            if (curImage1 != fntImg) {
                curImage1 = fntImg;
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, curImage1);
            }

            GLuint emjImg = curPaint.pathOp == TEXT && curPaint.emojis && FlatVectors::getEmoji() != nullptr ?
                    FlatVectors::getEmoji()->getEmojiImage() : 0;

            if (curImage2 != emjImg) {
                curImage2 = emjImg;
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, curImage2);
            }

            if (debug) {
                glStencilFunc(GL_ALWAYS, 0x00, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                _render_triangles(pos, renderElements);

            } else if (curPaint.pathOp == TEXT || curPaint.pathOp == CONVEX || curPaint.convex) {
                glStencilFunc(GL_EQUAL, 0x00, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                _render_triangles(pos, renderElements);

            } else if (curPaint.pathOp == FILL) {

                if (curPaint.pathRule == EVEN_ODD || curPaint.pathRule == NON_ZERO) {
                    // Even-Odd

                    glUniform1i(stcID, 1);
                    glColorMask(0, 0, 0, 0);
                    glStencilFunc(GL_NOTEQUAL, 0x80, 0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                    _render_triangles(pos, renderElements);

                    glUniform1i(stcID, 0);
                    glColorMask(1, 1, 1, 1);
                    glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                    _render_triangles(pos, renderElements);
                } else {
                    // Non-Zero

                    glUniform1i(stcID, 1);
                    glColorMask(0, 0, 0, 0);
                    glStencilFuncSeparate(GL_FRONT, GL_NOTEQUAL, 0x80, 0xFF);
                    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
                    glStencilFuncSeparate(GL_BACK, GL_NOTEQUAL, 0x80, 0xFF);
                    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
                    glStencilMask(0x7F);
                    _render_triangles(pos, renderElements);

                    glUniform1i(stcID, 0);
                    glColorMask(1, 1, 1, 1);
                    glStencilFunc(GL_GREATER, 0x00, 0x7F);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
                    glStencilMask(0x7F);
                    _render_triangles(pos, renderElements);
                }

                glStencilFunc(GL_EQUAL, 0x00, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            } else if (curPaint.pathOp == STROKE) {
                glUniform1i(stcID, 1);
                glColorMask(0, 0, 0, 0);
                glStencilFunc(GL_EQUAL, 0x00, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                _render_triangles(pos, renderElements);

                glUniform1i(stcID, 0);
                glColorMask(1, 1, 1, 1);
                glStencilFunc(GL_EQUAL, 0xFF, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
                _render_triangles(pos, renderElements);

                glStencilFunc(GL_EQUAL, 0x00, 0xFF);
                glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            }
        }

        pos = totalElements;
    }
}

// -- Font -- //

uint32 FlatRender::createFontTexture(int32 width, int32 height) {
    glActiveTexture(GL_TEXTURE0);

    GLuint img;
    glGenTextures(1, &img);
    glBindTexture(GL_TEXTURE_2D, img);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    curImage0 = 0;

    return img;
}

uint32 FlatRender::resizeFontTexture(uint32 oldImageID, int32 oldWidth, int32 oldHeight, int32 width, int32 height) {
    glActiveTexture(GL_TEXTURE0);

    GLuint newImg;
    glGenTextures(1, &newImg);
    glBindTexture(GL_TEXTURE_2D, newImg);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, oldImageID);

    unsigned char* oldData = (unsigned char *) malloc(oldWidth * oldHeight * sizeof(unsigned char));

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, oldData);

    glBindTexture(GL_TEXTURE_2D, newImg);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, oldWidth, oldHeight, GL_RED, GL_UNSIGNED_BYTE, oldData);

    free(oldData);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    curImage2 = 0;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    curImage1 = 0;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    curImage0 = 0;

    GLuint oldImgID = oldImageID;
    glDeleteTextures(1, &oldImgID);

    return newImg;
}

void FlatRender::updateFontTexture(uint32 imageID, void* data, int32 x, int32 y, int32 width, int32 height) {
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, imageID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    curImage0 = 0;

}

void FlatRender::destroyFontTexture(uint32 imageID) {
    if (imageID != 0) {
        GLuint imgID = imageID;
        glDeleteTextures(1, &imgID);
    }
}
