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
        "out vec4 FragColor;\n"
        "layout (std140) uniform Paint {\n"
        "    vec4 data;\n"
        "    mat3 colorMat;\n"
        "    mat3 imageMat;\n"
        "    vec4 shape;\n"
        "    vec4 extra;\n"
        "    vec4 stops[4];\n"
        "    vec4 colors[16];\n"
        "};\n"
        "uniform int stc;\n"
        "uniform int dbg;\n"
        "uniform sampler2D tex;\n"
        "uniform sampler2D fnt;\n"
        "uniform sampler2D emj;\n"
        "in vec2 oPos;\n"
        "in vec2 oTex;\n"
        "in float oEmj;\n"
        "float roundrect(vec2 pt, vec2 ext, float rad) {\n"
        "	 vec2 ext2 = ext - vec2(rad,rad);\n"
        "	 vec2 d = abs(pt) - ext2;\n"
        "	 return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;\n"
        "}\n"
        "float focuscircle(vec2 coord, vec2 focus) {\n"
        "    float gradLength = 1.0;\n"
        "    vec2 diff = focus;\n"
        "    vec2 rayDir = normalize(coord - focus);\n"
        "    float a = dot(rayDir, rayDir);\n"
        "    float b = 2.0 * dot(rayDir, diff);\n"
        "    float c = dot(diff, diff) - 1;\n"
        "    float disc = b * b - 4.0 * a * c;\n"
        "    if (disc >= 0.0) {\n"
        "        float t = (-b + sqrt(abs(disc))) / (2.0 * a);\n"
        "        vec2 projection = focus + rayDir * t;\n"
        "        gradLength = distance(projection, focus);\n"
        "    }\n"
        "    return distance(coord, focus) / gradLength;\n"
        "}\n"
        "float expin(float a, float power) {\n"
        "	 return 1 - pow(1 - a, power);\n"
        "}\n"
        "void main() {\n"
        "    if (stc == 1) {\n"
        "        FragColor = vec4(1);\n"
        "        return;\n"
        "    }\n"
        "    if (oEmj > 0 && dbg != 1) {\n"
        "        vec4 emjCol = texture(emj, oTex);\n"
        "        emjCol.a *= colors[0].a;\n"
        "        FragColor = vec4(emjCol.rgb * emjCol.a, emjCol.a);\n"
        "        return;\n"
        "    }\n"
        "    vec4 color = colors[0];\n"
        "    if (data[3] > 0) {\n"
        "        vec2 cPt = (colorMat * vec3(oPos, 1.0)).xy;\n"
        "        float t;\n"
        "        if (extra[2] == 0) {\n"
        "            t = (roundrect(cPt, shape.xy, shape.z) + shape.w * 0.5) / shape.w;\n"
        "        } else {\n"
        "            t = focuscircle(cPt / (shape.z * 2.0), extra.xy);\n"
        "        }\n"
        "        if (data[2] == 0) {\n"
        "            t = clamp(t, 0.0, 1.0);\n"
        "        } else if (data[2] == 1) {\n"
        "            t = t - floor(t);\n"
        "        } else if (data[2] == 2) {\n"
        "            t = (int(t) % 2 == 0) ? t - floor(t) : 1 - (t - floor(t));\n"
        "        } else {\n"
        "            t = expin(clamp(t, 0.0, 1.0), 2);\n"
        "        }\n"
        "        color = mix(color, colors[1], clamp((t - stops[0].x) / (stops[0].y - stops[0].x), 0.0, 1.0));\n"
        "        if(data[3] > 1) color = mix(color, colors[2], clamp((t - stops[0].y) / (stops[0].z - stops[0].y), 0.0, 1.0));\n"
        "        if(data[3] > 2) color = mix(color, colors[3], clamp((t - stops[0].z) / (stops[0].w - stops[0].z), 0.0, 1.0));\n"
        "        if(data[3] > 3) color = mix(color, colors[4], clamp((t - stops[0].w) / (stops[1].x - stops[0].w), 0.0, 1.0));\n"
        "        if(data[3] > 4) color = mix(color, colors[5], clamp((t - stops[1].x) / (stops[1].y - stops[1].x), 0.0, 1.0));\n"
        "        if(data[3] > 5) color = mix(color, colors[6], clamp((t - stops[1].y) / (stops[1].z - stops[1].y), 0.0, 1.0));\n"
        "        if(data[3] > 6) color = mix(color, colors[7], clamp((t - stops[1].z) / (stops[1].w - stops[1].z), 0.0, 1.0));\n"
        "        if(data[3] > 7) color = mix(color, colors[8], clamp((t - stops[1].w) / (stops[2].x - stops[1].w), 0.0, 1.0));\n"
        "        if(data[3] > 8) color = mix(color, colors[9], clamp((t - stops[2].x) / (stops[2].y - stops[2].x), 0.0, 1.0));\n"
        "        if(data[3] > 9) color = mix(color, colors[10], clamp((t - stops[2].y) / (stops[2].z - stops[2].y), 0.0, 1.0));\n"
        "        if(data[3] >10) color = mix(color, colors[11], clamp((t - stops[2].z) / (stops[2].w - stops[2].z), 0.0, 1.0));\n"
        "        if(data[3] >11) color = mix(color, colors[12], clamp((t - stops[2].w) / (stops[3].x - stops[2].w), 0.0, 1.0));\n"
        "        if(data[3] >12) color = mix(color, colors[13], clamp((t - stops[3].x) / (stops[3].y - stops[3].x), 0.0, 1.0));\n"
        "        if(data[3] >13) color = mix(color, colors[14], clamp((t - stops[3].y) / (stops[3].z - stops[3].y), 0.0, 1.0));\n"
        "        if(data[3] >14) color = mix(color, colors[15], clamp((t - stops[3].z) / (stops[3].w - stops[3].z), 0.0, 1.0));\n"
        "    }\n"
        "    if (data[0] == 1 || data[0] == 3) {\n"
        "        ivec2 sz = textureSize(tex, 0);\n"
        "        vec2 tPt = (imageMat * vec3(oPos, 1.0)).xy;\n"
        "        if (data[2] == 0 || data[2] == 3) {\n"
        "            tPt = clamp(tPt, vec2(0.0), vec2(1.0));\n"
        "        } else if (data[2] == 1 || data[2] == 4) {\n"
        "            tPt = tPt - floor(tPt);\n"
        "        } else if (data[2] == 2 || data[2] == 5) {\n"
        "            tPt = vec2((int(tPt.x) % 2 == 0) ? tPt.x - floor(tPt.x) : 1 - (tPt.x - floor(tPt.x)),\n"
        "                       (int(tPt.y) % 2 == 0) ? tPt.y - floor(tPt.y) : 1 - (tPt.y - floor(tPt.y)));\n"
        "        }\n"
        "        if (data[2] > 2) {\n"
        "            tPt = (floor(tPt * sz) + 0.5) / sz;\n"
        "        }\n"
        "        color *= texture(tex, tPt);\n"
        "    }\n"
        "    float a = color.a;\n"
        "    if (data[0] > 1) {\n"
        "        ivec2 sz = textureSize(fnt, 0);\n"
        "        float dist = texture(fnt, oTex / sz).r;\n"
        "        if (dbg == 1) {\n"
        "            a = 1;\n"
        "        } else if (extra[3] >= 0) {\n"
        "            float screenSpaceScale = fwidth(oTex).x * 0.05 + extra[3] * 0.5;\n"
        "            float aliasing = smoothstep(0.5 - screenSpaceScale, 0.5 + screenSpaceScale, dist);\n"
        "            a = a * aliasing;\n"
        "        } else {\n"
        "            a = a * dist;\n"
        "        }\n"
        "    }\n"
        "    FragColor = vec4(color.rgb * a, a);\n"
        "}\0";

// Local Private

int32 _get_align() {
    GLint align;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &align);
    return (GLint) ((sizeof(fvUniform) - 1) / align + 1) * align;
}

void _render_triangles(int32 pos, int32 length) {
    glDrawElements(GL_TRIANGLES, (GLsizei) (length), GL_UNSIGNED_INT, (void*) (pos * sizeof(int32)));
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
