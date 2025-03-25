//
// Created by Rodrigo on 30/11/2017
//

#include "flat_backend_SVG.h"
#include <memory>

#include "../flat_base_types.h"
#include "../flatvectors/FlatVectorsBase.h"
#include "../flatvectors/FlatVectors.h"
#include "../flatvectors/FlatPaints.h"
#include "../flatvectors/FlatFont.h"
#include "../flatvectors/FlatFontRender.h"
#include "../flatvectors/FlatImage.h"
#include "../flatvectors/FlatEmoji.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

FlatVectors* ctx(jlong ctx) {
    return  reinterpret_cast<FlatVectors*>(ctx);
}

FlatFont* fnt(jlong ctx) {
    return  reinterpret_cast<FlatFont*>(ctx);
}

FlatFontRender* fntCtx(jlong ctx) {
    return  reinterpret_cast<FlatFontRender*>(ctx);
}
//---------------------------
//         Context
//---------------------------
JNIEXPORT jlong JNICALL Java_flat_backend_SVG_Create(JNIEnv * jEnv, jclass jClass) {
    return reinterpret_cast<jlong>(new FlatVectors());
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_Destroy(JNIEnv * jEnv, jclass jClass, jlong context) {
    delete(ctx(context));
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetDebug (JNIEnv * jEnv, jclass jClass, jboolean debug) {
    FlatVectors::setDebug(debug == 1);
}

//---------------------------
//          Frame
//---------------------------
JNIEXPORT void JNICALL Java_flat_backend_SVG_BeginFrame(JNIEnv * jEnv, jclass jClass, jlong context, jint width, jint height) {
    ctx(context)->beginFrame(width, height);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_EndFrame(JNIEnv * jEnv, jclass jClass, jlong context) {
    ctx(context)->endFrame();
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_Flush(JNIEnv * jEnv, jclass jClass, jlong context) {
    ctx(context)->flush();
}

//---------------------------
//      Render styles
//---------------------------
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetAntiAlias(JNIEnv * jEnv, jclass jClass, jlong context, jboolean enabled) {
    ctx(context)->setAntiAliasing(enabled == 1 ? 1 : 0);
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetStroke(JNIEnv * jEnv, jclass jClass, jlong context, jfloat width, jint cap, jint join, jfloat miter, jfloatArray dash, jfloat dashPhase) {
    fvCap _cap = cap == 0 ? fvCap::CAP_BUTT : cap == 1 ? fvCap::CAP_SQUARE : fvCap::CAP_ROUND;
    fvJoin _join = join == 0 ? fvJoin::JOIN_BEVEL : join == 1 ? fvJoin::JOIN_MITER : fvJoin::JOIN_ROUND;

    FlatStroke stroke = FlatStroke(width, miter, _cap, _join);
    if (dash != 0) {
        int32 length = jEnv->GetArrayLength(dash);
        float* data = (float *) jEnv->GetPrimitiveArrayCritical(dash, 0);
        for (int32 i = 0; i < length; ++i) {
            stroke.dash.push_back(data[i]);
        }
        jEnv->ReleasePrimitiveArrayCritical(dash, data, 0);
    }
    ctx(context)->setStroke(stroke);
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetPaintColor(JNIEnv * jEnv, jclass jClass, jlong context, jint color) {
    fvUniform uniform;
    FlatPaints::setColorPaint(uniform, color);
    ctx(context)->setColor(uniform, 0);
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetPaintLinearGradient(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x1, jfloat y1, jfloat x2, jfloat y2, jint count, jfloatArray data, jint cycleMethod) {
    float* _data = (float*) jEnv->GetPrimitiveArrayCritical(data, 0);
    fvUniform uniform;
    FlatPaints::setLinearGradientPaint(uniform, _data, x1, y1, x2, y2, count, _data + 6, (int32*)(_data + 6 + 16), cycleMethod);
    ctx(context)->setColor(uniform, 0);
    jEnv->ReleasePrimitiveArrayCritical(data, _data, 0);
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetPaintRadialGradient(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x1, jfloat y1, jfloat fx, jfloat fy, jfloat rIn, jfloat rOut, jint count, jfloatArray data, jint cycleMethod) {
    float* _data = (float*) jEnv->GetPrimitiveArrayCritical(data, 0);
    fvUniform uniform;
    FlatPaints::setRadialGradientPaint(uniform, _data, x1, y1, rIn, rOut, fx, fy, count, _data + 6, (int32*)(_data + 6 + 16), cycleMethod);
    ctx(context)->setColor(uniform, 0);
    jEnv->ReleasePrimitiveArrayCritical(data, _data, 0);
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetPaintBoxGradient(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jfloat w, jfloat h, jfloat corners, jfloat blur, jfloat alpha, jint color, jfloatArray data) {
    float* _data = data == NULL ? 0 : (float*) jEnv->GetPrimitiveArrayCritical(data, 0);
    fvUniform uniform;
    FlatPaints::setBoxGradientPaint(uniform, _data, x, y, w, h, corners, blur, alpha, color);
    ctx(context)->setColor(uniform, 0);
    if (data != NULL) {
        jEnv->ReleasePrimitiveArrayCritical(data, _data, 0);
    }
}

JNIEXPORT void JNICALL Java_flat_backend_SVG_SetPaintImage(JNIEnv * jEnv, jclass jClass, jlong context, jint imageID, jint color, jfloatArray data, jint cycleMethod) {
    float* _data = (float*) jEnv->GetPrimitiveArrayCritical(data, 0);
    fvUniform uniform;
    FlatPaints::setImagePaint(uniform, _data, color, cycleMethod);
    ctx(context)->setColor(uniform, imageID);
    jEnv->ReleasePrimitiveArrayCritical(data, _data, 0);
}

//---------------------------
//         Transforms
//---------------------------
JNIEXPORT void JNICALL Java_flat_backend_SVG_TransformIdentity(JNIEnv * jEnv, jclass jClass, jlong context) {
    ctx(context)->setTransform(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_TransformSet(JNIEnv * jEnv, jclass jClass, jlong context, jfloat m00, jfloat m01, jfloat m10, jfloat m11, jfloat m02, jfloat m12) {
    ctx(context)->setTransform(m00, m10, m01, m11, m02, m12);
}

//---------------------------
//          Clipping
//---------------------------
JNIEXPORT void JNICALL Java_flat_backend_SVG_ClearClip(JNIEnv * jEnv, jclass jClass, jlong context, jint enabled) {
    ctx(context)->clearClip(enabled == 1);
}
//---------------------------
//           Paths
//---------------------------
JNIEXPORT void JNICALL Java_flat_backend_SVG_PathBegin(JNIEnv * jEnv, jclass jClass, jlong context, jint type, jint rule) {
    fvPathOp op = type == 0 ? fvPathOp::FILL :
                  type == 1 ? fvPathOp::STROKE :
                  type == 2 ? fvPathOp::CLIP : fvPathOp::UNCLIP;

    fvWindingRule wr = rule == 0 ? fvWindingRule::EVEN_ODD : fvWindingRule::NON_ZERO;

    ctx(context)->begin(op, wr);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_MoveTo(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y) {
    ctx(context)->moveTo(x, y);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_LineTo(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y) {
    ctx(context)->lineTo(x, y);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_CubicTo(JNIEnv * jEnv, jclass jClass, jlong context, jfloat c1x, jfloat c1y, jfloat c2x, jfloat c2y, jfloat x, jfloat y) {
    ctx(context)->cubicTo(c1x, c1y, c2x, c2y, x, y);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_QuadTo(JNIEnv * jEnv, jclass jClass, jlong context, jfloat cx, jfloat cy, jfloat x, jfloat y) {
    ctx(context)->quadTo(cx, cy, x, y);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_Close(JNIEnv * jEnv, jclass jClass, jlong context) {
    ctx(context)->close();
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_PathEnd(JNIEnv * jEnv, jclass jClass, jlong context) {
    ctx(context)->end();
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_Rect(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jfloat width, jfloat height, jboolean fill) {
    ctx(context)->rect(x, y, width, height, fill);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_Ellipse(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jfloat width, jfloat height, jboolean fill) {
    ctx(context)->ellipse(x, y, width, height, fill);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_RoundRect(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jfloat width, jfloat height, jfloat c1, jfloat c2, jfloat c3, jfloat c4, jboolean fill) {
    ctx(context)->roundRect(x, y, width, height, c1, c2, c3, c4, fill);
}
//---------------------------
//           Text
//---------------------------
JNIEXPORT jlong JNICALL Java_flat_backend_SVG_FontLoad(JNIEnv * jEnv, jclass jClass, jbyteArray data, jfloat size, jint sdf) {
    int32 length = jEnv->GetArrayLength(data);
    void* _data = jEnv->GetPrimitiveArrayCritical(data, 0);
    FlatFont* font = new FlatFont(_data, length, size, sdf == 1);
    jEnv->ReleasePrimitiveArrayCritical(data, _data, 0);

    if (!font->isValid()) {
        delete font;
        return 0;
    }
    return reinterpret_cast<jlong>(font);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontUnload(JNIEnv * jEnv, jclass jClass, jlong font) {
    delete(fnt(font));
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontCreateEmoji(JNIEnv * jEnv, jclass jClass, jint textureId, jintArray sequence) {
    int32 length = jEnv->GetArrayLength(sequence);
    int32* _data = (int32*) jEnv->GetPrimitiveArrayCritical(sequence, 0);

    // Clear Before
    delete FlatVectors::getEmoji();

    FlatEmoji* emoji = new FlatEmoji(textureId, _data, length);
    FlatVectors::setEmoji(emoji);
    jEnv->ReleasePrimitiveArrayCritical(sequence, _data, 0);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontDestroyEmoji(JNIEnv * jEnv, jclass jClass) {
    FlatEmoji* emoji = FlatVectors::getEmoji();
    delete emoji;
    FlatVectors::setEmoji(nullptr);
}
JNIEXPORT jlong JNICALL Java_flat_backend_SVG_FontPaintCreate(JNIEnv * jEnv, jclass jClass, jlong context, jlong font) {
    FlatFontRender* fontCtx = new FlatFontRender(fnt(font), ctx(context)->getRender());
    return reinterpret_cast<jlong>(fontCtx);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontPaintDestroy(JNIEnv * jEnv, jclass jClass, jlong fontPaint) {
    delete(fntCtx(fontPaint));
}
JNIEXPORT jlong JNICALL Java_flat_backend_SVG_FontPaintGetAtlas(JNIEnv * jEnv, jclass jClass, jlong fontPaint, jintArray size) {
    int32 w, h;
    int32 imageID = fntCtx(fontPaint)->getCurrentAtlas(&w, &h);

    jint imageInfo[2];
    imageInfo[0] = w;
    imageInfo[1] = h;
    jEnv->SetIntArrayRegion(size, 0, 2, imageInfo);

    return imageID;
}
JNIEXPORT jfloatArray JNICALL Java_flat_backend_SVG_FontGetGlyphShape(JNIEnv * jEnv, jclass jClass, jlong font, jint unicode) {
    std::vector<float> polygon;
    fnt(font)->getGlyphShape(unicode, polygon);

    if (polygon.empty()) {
        return NULL;
    } else {
        jfloatArray imageArray = jEnv->NewFloatArray(polygon.size());
        jEnv->SetFloatArrayRegion(imageArray, 0, polygon.size(), polygon.data());
        return imageArray;
    }
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetGlyph(JNIEnv * jEnv, jclass jClass, jlong font, jint codePoint, jfloatArray glyph) {
    float data[8];
    fnt(font)->getGlyphData(codePoint, data);
    jEnv->SetFloatArrayRegion(glyph, 0, 8, (jfloat *)data);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetAllCodePoints(JNIEnv * jEnv, jclass jClass, jlong font, jintArray codePoints) {
    jsize size = jEnv->GetArrayLength(codePoints);
    std::vector<int32> data;
    fnt(font)->getAllCodePoints(data);
    jEnv->SetIntArrayRegion(codePoints, 0, size, data.data());
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetHeight(JNIEnv * jEnv, jclass jClass, jlong font) {
    jfloat height;
    fnt(font)->getMetrics(0, 0, &height, 0, 0);
    return height;
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetAscent(JNIEnv * jEnv, jclass jClass, jlong font) {
    jfloat ascent;
    fnt(font)->getMetrics(&ascent, 0, 0, 0, 0);
    return ascent;
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetDescent(JNIEnv * jEnv, jclass jClass, jlong font) {
    jfloat descent;
    fnt(font)->getMetrics(0, &descent, 0, 0, 0);
    return descent;
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetLineGap(JNIEnv * jEnv, jclass jClass, jlong font) {
    jfloat lineGap;
    fnt(font)->getMetrics(0, 0, 0, &lineGap, 0);
    return lineGap;
}
JNIEXPORT jint JNICALL Java_flat_backend_SVG_FontGetGlyphCount(JNIEnv * jEnv, jclass jClass, jlong font) {
    int32 glyphCount;
    fnt(font)->getMetrics(0, 0, 0, 0, &glyphCount);
    return (jint) glyphCount;
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetTextWidth(JNIEnv * jEnv, jclass jClass, jlong font, jstring characters, jfloat scale, jfloat spacing) {
    const char *chars = jEnv->GetStringUTFChars(characters, 0);
    jfloat width = fnt(font)->getTextWidth(chars, jEnv->GetStringUTFLength(characters), scale, spacing);
    jEnv->ReleaseStringUTFChars(characters, chars);
    return width;
}
JNIEXPORT jfloat JNICALL Java_flat_backend_SVG_FontGetTextWidthBuffer(JNIEnv * jEnv, jclass jClass, jlong font, jobject characters, jint offset, jint length, jfloat scale, jfloat spacing) {
    const char * chars = (const char *) (jEnv->GetDirectBufferAddress(characters)) + offset;
    return fnt(font)->getTextWidth(chars, length, scale, spacing);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetOffset(JNIEnv * jEnv, jclass jClass, jlong font, jstring characters, jfloat scale, jfloat spacing, jfloat x, jboolean half, jfloatArray cursor) {
    const char *chars = jEnv->GetStringUTFChars(characters, 0);
    float data[2] = {0, 1};
    fnt(font)->getOffset(chars, jEnv->GetStringUTFLength(characters), scale, spacing, x, half, data, data + 1);
    jEnv->ReleaseStringUTFChars(characters, chars);
    jEnv->SetFloatArrayRegion(cursor, 0, 2, (jfloat *)data);

}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetOffsetBuffer(JNIEnv * jEnv, jclass jClass, jlong font, jobject characters, jint offset, jint length, jfloat scale, jfloat spacing, jfloat x, jboolean half, jfloatArray cursor) {
    const char * chars = (const char *) (jEnv->GetDirectBufferAddress(characters)) + offset;
    float data[2] = {0, 1};
    fnt(font)->getOffset(chars, length, scale, spacing, x, half, data, data + 1);
    jEnv->SetFloatArrayRegion(cursor, 0, 2, (jfloat *)data);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetOffsetSpace(JNIEnv * jEnv, jclass jClass, jlong font, jstring characters, jfloat scale, jfloat spacing, jfloat x, jfloatArray cursor) {
    const char *chars = jEnv->GetStringUTFChars(characters, 0);
    float data[2] = {0, 1};
    fnt(font)->getOffsetSpace(chars, jEnv->GetStringUTFLength(characters), scale, spacing, x, data, data + 1);
    jEnv->ReleaseStringUTFChars(characters, chars);
    jEnv->SetFloatArrayRegion(cursor, 0, 2, (jfloat *)data);

}
JNIEXPORT void JNICALL Java_flat_backend_SVG_FontGetOffsetSpaceBuffer(JNIEnv * jEnv, jclass jClass, jlong font, jobject characters, jint offset, jint length, jfloat scale, jfloat spacing, jfloat x, jfloatArray cursor) {
    const char * chars = (const char *) (jEnv->GetDirectBufferAddress(characters)) + offset;
    float data[2] = {0, 1};
    fnt(font)->getOffsetSpace(chars, length, scale, spacing, x, data, data + 1);
    jEnv->SetFloatArrayRegion(cursor, 0, 2, (jfloat *)data);
}
JNIEXPORT jint JNICALL Java_flat_backend_SVG_FontGetLineWrap(JNIEnv * jEnv, jclass jClass, jlong font, jstring characters, jfloat scale, jfloat spacing, jfloat x) {
    const char *chars = jEnv->GetStringUTFChars(characters, 0);
    jint count = fnt(font)->countLineWrap(chars, jEnv->GetStringUTFLength(characters), scale, spacing, x);
    jEnv->ReleaseStringUTFChars(characters, chars);
    return count;

}
JNIEXPORT jint JNICALL Java_flat_backend_SVG_FontGetLineWrapBuffer(JNIEnv * jEnv, jclass jClass, jlong font, jobject characters, jint offset, jint length, jfloat scale, jfloat spacing, jfloat x) {
    const char * chars = (const char *) (jEnv->GetDirectBufferAddress(characters)) + offset;
    return fnt(font)->countLineWrap(chars, length, scale, spacing, x);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetFont(JNIEnv * jEnv, jclass jClass, jlong context, jlong fontPaint) {
    ctx(context)->setFont(fntCtx(fontPaint));
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetFontScale(JNIEnv * jEnv, jclass jClass, jlong context, jfloat scale) {
    ctx(context)->setFontScale(scale);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetFontSpacing(JNIEnv * jEnv, jclass jClass, jlong context, jfloat spacing) {
    ctx(context)->setFontSpacing(spacing);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_SetFontBlur(JNIEnv * jEnv, jclass jClass, jlong context, jfloat blur) {
    ctx(context)->setFontBlur(blur);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_DrawText(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jstring characters, jfloat maxWidth, jfloat maxHeight) {
    const char *chars = jEnv->GetStringUTFChars(characters, 0);
    ctx(context)->text(chars, jEnv->GetStringUTFLength(characters), x, y, maxWidth, maxHeight);
    jEnv->ReleaseStringUTFChars(characters, chars);
}
JNIEXPORT void JNICALL Java_flat_backend_SVG_DrawTextBuffer(JNIEnv * jEnv, jclass jClass, jlong context, jfloat x, jfloat y, jobject characters, jint offset, jint length, jfloat maxWidth, jfloat maxHeight) {
    const char * chars = (const char *) (jEnv->GetDirectBufferAddress(characters)) + offset;
    ctx(context)->text(chars, length, x, y, maxWidth, maxHeight);
}
JNIEXPORT jbyteArray JNICALL Java_flat_backend_SVG_ReadImage(JNIEnv * jEnv, jclass jClass, jbyteArray data, jintArray imageData) {
    jbyte *imageBytes = jEnv->GetByteArrayElements(data, NULL);
    jsize imageSize = jEnv->GetArrayLength(data);

    int width, height, nrChannels;
    unsigned char *imageDataBuffer = stbi_load_from_memory((unsigned char *)imageBytes, imageSize, &width, &height, &nrChannels, 4);

    if (imageDataBuffer == NULL) {
        jEnv->ReleaseByteArrayElements(data, imageBytes, JNI_ABORT);
        return NULL;
    }

    jint imageInfo[3];
    imageInfo[0] = width;
    imageInfo[1] = height;
    imageInfo[2] = nrChannels;

    jEnv->SetIntArrayRegion(imageData, 0, 3, imageInfo);

    jbyteArray imageArray = jEnv->NewByteArray(width * height * 4);
    jEnv->SetByteArrayRegion(imageArray, 0, width * height * 4, (jbyte *)imageDataBuffer);

    stbi_image_free(imageDataBuffer);

    jEnv->ReleaseByteArrayElements(data, imageBytes, JNI_ABORT);

    return imageArray;
}
JNIEXPORT jbyteArray JNICALL Java_flat_backend_SVG_WriteImage(JNIEnv * jEnv, jclass jClass, jbyteArray imageData, jint width, jint height, jint channels, jint format, jint quality) {
    jbyte *imageBytes = jEnv->GetByteArrayElements(imageData, NULL);

    int size;
    unsigned char* data = NULL;
    if (format == 0) {
        data = FlatImage::writePng((unsigned char *) imageBytes, width, height, channels, &size);
    } else if (format == 1) {
        data = FlatImage::writeJpg((unsigned char *) imageBytes, width, height, channels, quality, &size);
    } else if (format == 2) {
        data = FlatImage::writeBmp((unsigned char *) imageBytes, width, height, channels, &size);
    } else if (format == 3) {
        data = FlatImage::writeTga((unsigned char *) imageBytes, width, height, channels, &size);
    }

    if (data == NULL) {
        return NULL;
    }

    jbyteArray dataArrey = jEnv->NewByteArray(size);
    jEnv->SetByteArrayRegion(dataArrey, 0, size, (jbyte *)data);

    if (format == 0) {
        FlatImage::freePng(data);
    } else if (format == 1) {
        FlatImage::freeJpg(data);
    } else if (format == 2) {
        FlatImage::freeBmp(data);
    } else if (format == 3) {
        FlatImage::freeTga(data);
    }

    return dataArrey;
}