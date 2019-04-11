//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/1/2
// Created by: Zed
//


#include <assert.h>
#include <stdio.h>

#include "MTGLDebug_Macro.h"
#include "MTGLDebug_Platform.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE
#include <objc/runtime.h>
#endif


#ifdef MTGLDEBUG_PLATFORM_ANDROID
#include <malloc.h>
#include "xhook.h"
#endif

#include "MTGLDebugCoreBridge.hpp"
#include "MTGLDebugGLContextCInterface.h"
#include "MTGLDebugGLFunction.h"
#include "MTGLDebugGLHookFunction.h"
/**
 Texture 	1 << 1  MTGLDebugCoreObjectTexture
 CVObject 	1 << 2  MTGLDebugCoreObjectCVTexture
 Program 	1 << 3  MTGLDebugCoreObjectProgram
 其他基础类型 1 << 0  MTGLDebugCoreObjectBaseClass
 */

void exception(const char *methodName, GLuint error) {
    MTGLDebugCore_Excption(methodName, error);
}

#define mtgldebug_func(M_NAME, RETURN_TYPE, ...) \
    RETURN_TYPE (*func)                          \
    (__VA_ARGS__) = mtgldebug_Get##M_NAME();

#define mtgldebug_execute(M_NAME, ...)                                      \
    if (MTGLDebugCore_IsEnableCheckAPIUsageStates()) {                      \
        if (!currentContextIsExist()) {                                     \
            exception("currentContextIsNotExist", currentContextIsExist()); \
        }                                                                   \
        GLuint err = glGetError();                                          \
        if (err) {                                                          \
            exception("glGetError", err);                                   \
        }                                                                   \
        _Pragma("clang diagnostic push")                                    \
            _Pragma("clang diagnostic ignored \"-Wstrict-prototypes\"")     \
                func(__VA_ARGS__);                                          \
        err = glGetError();                                                 \
        if (err) {                                                          \
            exception(#M_NAME, err);                                        \
        }                                                                   \
    } else {                                                                \
        func(__VA_ARGS__);                                                  \
    }                                                                       \
    _Pragma("clang diagnostic pop")


//glGetError()是EGL自带的错误检测函数
#define mtgldebug_execute_return(M_NAME, RESULT, ...)                       \
    if (MTGLDebugCore_IsEnableCheckAPIUsageStates()) {                      \
        if (!currentContextIsExist()) {                                     \
            exception("currentContextIsNotExist", currentContextIsExist()); \
        }                                                                   \
        GLuint err = glGetError();                                          \
        if (err) {                                                          \
            exception("glGetError", err);                                   \
        }                                                                   \
        _Pragma("clang diagnostic push")                                    \
            _Pragma("clang diagnostic ignored \"-Wstrict-prototypes\"")     \
                RESULT                                                      \
            = func(__VA_ARGS__);                                            \
        err = glGetError();                                                 \
        if (err) {                                                          \
            exception(#M_NAME, err);                                        \
        }                                                                   \
    } else {                                                                \
        RESULT = func(__VA_ARGS__);                                         \
    }                                                                       \
    _Pragma("clang diagnostic pop")



/** Texture **/
void mtgldebug_glGenTextures(GLsizei n, GLuint *textures) {
    mtgldebug_func(glGenTextures, void, GLsizei, GLuint *)
        mtgldebug_execute(glGenTextures, n, textures)

            for (GLsizei i = 0; i < n; i++) {
        MTGLDebugCore_AddObject(textures[i], MT_GLDEBUG_TEXTURE, sizeof(GLuint), MTGLDebugCoreObjectTexture, NULL);
    }
}
void mtgldebug_glBindTexture(GLenum target, GLuint texture) {
    mtgldebug_func(glBindTexture, void, GLenum, GLuint)
        mtgldebug_execute(glBindTexture, target, texture)
            MTGLDebugCore_CheckObject(texture, MT_GLDEBUG_TEXTURE, NULL, MTGLDebugCoreObjectTexture, NULL);
}

void mtgldebug_glTexImage2D(GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLenum format,
    GLenum type,
    const GLvoid *pixels) {
    GLint object = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &object);
    mtgldebug_func(glTexImage2D,
        void,
        GLenum,
        GLint,
        GLint,
        GLsizei,
        GLsizei,
        GLint,
        GLenum,
        GLenum,
        const GLvoid *)
        mtgldebug_execute(glTexImage2D, target, level, internalformat, width, height, border, format, type, pixels)

            MTGLDebugCore_CheckObject(object, MT_GLDEBUG_TEXTURE, NULL, MTGLDebugCoreObjectTexture, NULL);

    MTGLDebugCore_TextureOutputObjectSetSize(MT_GLDEBUG_TEXTURE, object, width, height);

    size_t memorySize = MTGLDebugCore_CalculateMemorySize(width, height, format, type);
    MTGLDebugCore_OutputObjectSetMemorySize(MT_GLDEBUG_TEXTURE, object, memorySize + sizeof(GLuint));
}

GLuint mtgldebug_pixelformatGetBytesPerPixel(GLenum format) {
    switch (format) {
        case GL_ALPHA:
            return 1;
        case GL_LUMINANCE:
            return 1;
        case GL_RED_EXT:
            return 1;
        case GL_RGBA8_OES:
            return 1;

        case GL_LUMINANCE_ALPHA:
            return 2;
        case GL_RG_EXT:
            return 2;
        case GL_RGB565:
            return 2;
        case GL_RGB:
            return 3;

        case GL_RGBA4:
            return 2;
        case GL_RGB5_A1:
            return 2;
        case GL_RGBA:
            return 4;

#ifdef MTGLDEBUG_PLATFORM_ANDROID
        case GL_BGRA_EXT:
            return 4;
#else
        case GL_BGRA:
            return 4;
#endif
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            return 0;
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            return 0;
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            return 0;
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            return 0;
        default:
            return 0;
    }
}


void mtgldebug_glTexSubImage2D(GLenum target,
    GLint level,
    GLint xoffset,
    GLint yoffset,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    const GLvoid *pixels) {

    GLint object = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &object);

    GLint value;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &value);
    //区别 UNPACK_ALIGNMENT 和 PACK_ALIGNMENT
    //这个异常是不会导致程序崩溃，但是会导致图像信息错误

    GLint byesPerPixel = mtgldebug_pixelformatGetBytesPerPixel(format);

    if (MTGLDebugCore_IsEnableGLDebugException() && (byesPerPixel % value)) {
        exception("glTexSubImage2D:所读取的纹理数据的行对齐方式不一致!", -1);
    }


    mtgldebug_func(glTexSubImage2D,
        void,
        GLenum,
        GLint,
        GLint,
        GLint,
        GLsizei,
        GLsizei,
        GLenum,
        GLenum,
        const GLvoid *)
        mtgldebug_execute(glTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels)

    if (MTGLDebugCore_IsEnableGLDebugException()) {
        MTGLDebugCore_CheckObject(object, MT_GLDEBUG_TEXTURE, NULL, MTGLDebugCoreObjectTexture, NULL);
        MTGLDebugCore_TextureOutputObjectSetSize(MT_GLDEBUG_TEXTURE, object, width, height);
        
        size_t memorySize = MTGLDebugCore_CalculateMemorySize(width, height, format, type);
        MTGLDebugCore_OutputObjectSetMemorySize(MT_GLDEBUG_TEXTURE, object, memorySize + sizeof(GLuint));
    }
    else {
        if (mtgldebug_pixelformatGetBytesPerPixel(format) != value) {
            MTGLDebugCore_DeleteObject(object, MT_GLDEBUG_TEXTURE, MTGLDebugCoreObjectTexture);
        } else {
            
            MTGLDebugCore_CheckObject(object, MT_GLDEBUG_TEXTURE, NULL, MTGLDebugCoreObjectTexture, NULL);
            MTGLDebugCore_TextureOutputObjectSetSize(MT_GLDEBUG_TEXTURE, object, width, height);
            
            size_t memorySize = MTGLDebugCore_CalculateMemorySize(width, height, format, type);
            MTGLDebugCore_OutputObjectSetMemorySize(MT_GLDEBUG_TEXTURE, object, memorySize + sizeof(GLuint));
        }
    }
}

void mtgldebug_glDeleteTextures(GLsizei n, const GLuint *textures) {
    mtgldebug_func(glDeleteTextures, void, GLsizei, const GLuint *)
        mtgldebug_execute(glDeleteTextures, n, textures)
            MTGLDebugCore_DeleteObjects(n, textures, MT_GLDEBUG_TEXTURE, MTGLDebugCoreObjectTexture);
}


/** Framebuffer **/
void mtgldebug_glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
    mtgldebug_func(glGenFramebuffers, void, GLsizei, GLuint *)
        mtgldebug_execute(glGenFramebuffers, n, framebuffers)

            for (GLsizei i = 0; i < n; i++) {
        MTGLDebugCore_AddObject(framebuffers[i], MT_GLDEBUG_FRAMEBUFFER, sizeof(GLuint), MTGLDebugCoreObjectFramebuffer, NULL);
    }
}

void mtgldebug_glBindFramebuffer(GLenum target, GLuint framebuffer) {
    mtgldebug_func(glBindFramebuffer, void, GLenum, GLuint)
        mtgldebug_execute(glBindFramebuffer, target, framebuffer)
            MTGLDebugCore_CheckObject(framebuffer, MT_GLDEBUG_FRAMEBUFFER, NULL, MTGLDebugCoreObjectFramebuffer, NULL);
}

void mtgldebug_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    GLint object = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &object);
    mtgldebug_func(glFramebufferTexture2D, void, GLenum, GLenum, GLenum, GLuint, GLint)
        mtgldebug_execute(glFramebufferTexture2D, target, attachment, textarget, texture, level)

            MTGLDebugCore_FramebufferObjectSetTexture(MT_GLDEBUG_FRAMEBUFFER, object, texture);
}

void mtgldebug_glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
    mtgldebug_func(glDeleteFramebuffers, void, GLsizei, const GLuint *)
        mtgldebug_execute(glDeleteFramebuffers, n, framebuffers);
    MTGLDebugCore_DeleteObjects(n, framebuffers, MT_GLDEBUG_FRAMEBUFFER, MTGLDebugCoreObjectFramebuffer);
}

void mtgldebug_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {

    mtgldebug_func(glFramebufferRenderbuffer, void, GLenum, GLenum, GLenum, GLuint)
        mtgldebug_execute(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
    GLint width = 0, height = 0;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);

    MTGLDebugCore_CheckObject(renderbuffer, GL_RENDERBUFFER, NULL, MTGLDebugCoreObjectTexture, NULL);
    MTGLDebugCore_TextureOutputObjectSetSize(GL_RENDERBUFFER, renderbuffer, width, height);
}


/** Buffers **/

void mtgldebug_glGenBuffers(GLsizei n, GLuint *buffers) {
    mtgldebug_func(glGenBuffers, void, GLsizei, GLuint *)
        mtgldebug_execute(glGenBuffers, n, buffers);

    for (GLsizei i = 0; i < n; i++) {
        MTGLDebugCore_AddObject(buffers[i], MT_GLDEBUG_BUFFER, sizeof(GLuint), MTGLDebugCoreObjectBase, NULL);
    }
}

void mtgldebug_glBindBuffer(GLenum target, GLuint buffer) {
    mtgldebug_func(glBindBuffer, void, GLenum, GLuint)
        mtgldebug_execute(glBindBuffer, target, buffer);
    MTGLDebugCore_CheckObject(buffer, MT_GLDEBUG_BUFFER, NULL, MTGLDebugCoreObjectBase, NULL);
}

void mtgldebug_glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) {
    GLint object = 0;
    glGetIntegerv(target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, &object);
    mtgldebug_func(glBufferData, void, GLenum, GLsizeiptr, const GLvoid *, GLenum)
        mtgldebug_execute(glBufferData, target, size, data, usage)

            MTGLDebugCore_CheckObject(object, MT_GLDEBUG_BUFFER, NULL, MTGLDebugCoreObjectBase, NULL);
    MTGLDebugCore_OutputObjectAppendMemorySize(MT_GLDEBUG_BUFFER, object, size);
}

void mtgldebug_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data) {
    GLint object = 0;
    glGetIntegerv(target == GL_ARRAY_BUFFER ? GL_ARRAY_BUFFER_BINDING : GL_ELEMENT_ARRAY_BUFFER_BINDING, &object);
    mtgldebug_func(glBufferSubData, void, GLenum, GLintptr, GLsizeiptr, const GLvoid *)
        mtgldebug_execute(glBufferSubData, target, offset, size, data)
            MTGLDebugCore_CheckObject(object, MT_GLDEBUG_BUFFER, NULL, MTGLDebugCoreObjectBase, NULL);
}

void mtgldebug_glDeleteBuffers(GLsizei n, const GLuint *buffers) {
    mtgldebug_func(glDeleteBuffers, void, GLsizei, const GLuint *)
        mtgldebug_execute(glDeleteBuffers, n, buffers);
    MTGLDebugCore_DeleteObjects(n, buffers, MT_GLDEBUG_BUFFER, MTGLDebugCoreObjectBase);
}


/** Renderbuffer **/
void mtgldebug_glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
    mtgldebug_func(glGenRenderbuffers, void, GLsizei, GLuint *)
        mtgldebug_execute(glGenRenderbuffers, n, renderbuffers);

    for (GLsizei i = 0; i < n; i++) {
        MTGLDebugCore_AddObject(renderbuffers[i], GL_RENDERBUFFER, sizeof(GLuint), MTGLDebugCoreObjectTexture, NULL);
    }
}

void mtgldebug_glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
    mtgldebug_func(glBindRenderbuffer, void, GLenum, GLuint)
        mtgldebug_execute(glBindRenderbuffer, target, renderbuffer);
    MTGLDebugCore_CheckObject(renderbuffer, target, NULL, MTGLDebugCoreObjectTexture, NULL);
}

void mtgldebug_glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers) {
    mtgldebug_func(glDeleteRenderbuffers, void, GLsizei, const GLuint *)
        mtgldebug_execute(glDeleteRenderbuffers, n, renderbuffers)
            MTGLDebugCore_DeleteObjects(n, renderbuffers, GL_RENDERBUFFER, MTGLDebugCoreObjectTexture);
}


/** Program **/

GLuint mtgldebug_glCreateProgram(void) {
    GLuint program;
    mtgldebug_func(glCreateProgram, GLuint, void)
        mtgldebug_execute_return(glCreateProgram, program)
            MTGLDebugCore_AddObject(program, MT_GLDEBUG_PROGRAM, sizeof(GLuint), MTGLDebugCoreObjectProgram, NULL);
    return program;
}

void mtgldebug_glLinkProgram(GLuint program) {
    mtgldebug_func(glLinkProgram, void, GLuint)
        mtgldebug_execute(glLinkProgram, program)

        //判断脚本是否验证成功
        GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) {
        GLsizei maxcount = 2;
        GLsizei count = 0;
        GLuint shaders[maxcount];
        GLsizei bufSize = 1024;
        glGetAttachedShaders(program, maxcount, &count, shaders);
        for (int i = 0; i < count; i++) {
            GLsizei shaderLength = 0;
            GLchar *source = malloc(sizeof(GLchar) * 1024);
            GLint params = 0;
            glGetShaderiv(shaders[i], GL_SHADER_TYPE, &params);
            glGetShaderSource(shaders[i], bufSize, &shaderLength, source);
            MTGLDebugCore_ProgramObjectSetShaderString(MT_GLDEBUG_PROGRAM, program, params, source, shaderLength);
            free(source);
        }
    } else {
        exception("program link failed!!!", -1);
    }
}

void mtgldebug_glUseProgram(GLuint program) {
    mtgldebug_func(glUseProgram, void, GLuint)
        mtgldebug_execute(glUseProgram, program);
    MTGLDebugCore_CheckObject(program, MT_GLDEBUG_PROGRAM, NULL, MTGLDebugCoreObjectProgram, NULL);
}

void mtgldebug_glDeleteProgram(GLuint program) {
    mtgldebug_func(glUseProgram, void, GLuint)
        mtgldebug_execute(glDeleteProgram, program);

    MTGLDebugCore_DeleteObject(program, MT_GLDEBUG_PROGRAM, MTGLDebugCoreObjectProgram);
}

void mtgldebug_glEnableVertexAttribArray(GLuint index) {
    mtgldebug_func(glEnableVertexAttribArray, void, GLuint)
        mtgldebug_execute(glEnableVertexAttribArray, index);
}

void mtgldebug_glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr) {
    mtgldebug_func(glVertexAttribPointer, void, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *)

        mtgldebug_execute(glVertexAttribPointer, indx, size, type, normalized, stride, ptr);
}

int mtgldebug_glGetUniformLocation(GLuint program, const GLchar *name) {
    int uniformHandler;
    mtgldebug_func(glGetUniformLocation, int, GLuint, const GLchar *)

        mtgldebug_execute_return(glGetUniformLocation, uniformHandler, program, name)

        //	MTGLDebugCore_AddObject(program, MT_GLDEBUG_PROGRAM, sizeof(GLuint), 0x0, NULL);
        return uniformHandler;
}

int mtgldebug_glGetAttribLocation(GLuint program, const GLchar *name) {
    int uniformHandler;
    mtgldebug_func(glGetAttribLocation, int, GLuint, const GLchar *)

        mtgldebug_execute_return(glGetAttribLocation, uniformHandler, program, name)

        //	MTGLDebugCore_AddObject(program, MT_GLDEBUG_PROGRAM, sizeof(GLuint), 0x0, NULL);
        return uniformHandler;
}



void mtgldebug_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) {
    mtgldebug_func(glDrawElements, void, GLenum, GLsizei, GLenum, const GLvoid *)
        mtgldebug_execute(glDrawElements, mode, count, type, indices);
    MTGLDebugCore_DidDraw();
}

void mtgldebug_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    mtgldebug_func(glDrawArrays, void, GLenum, GLint, GLsizei)
        mtgldebug_execute(glDrawArrays, mode, first, count);
    MTGLDebugCore_DidDraw();
}

void mtgldebug_glClear(GLbitfield mask) {
    mtgldebug_func(glClear, void, GLbitfield)
        mtgldebug_execute(glClear, mask);
}

void mtgldebug_glUniform1i(GLint location, GLint x) {
    mtgldebug_func(glUniform1i, void, GLint, GLint)
        mtgldebug_execute(glUniform1i, location, x);
}

void mtgldebug_glUniform2f(GLint location, GLfloat x, GLfloat y) {

    mtgldebug_func(glUniform2f, void, GLint, GLfloat, GLfloat)
        mtgldebug_execute(glUniform2f, location, x, y);
}

void mtgldebug_glUniform1f(GLint location, GLfloat x) {
    mtgldebug_func(glUniform1f, void, GLint, GLfloat)
        mtgldebug_execute(glUniform1f, location, x);
}

void mtgldebug_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
    mtgldebug_func(glUniformMatrix4fv, void, GLint, GLsizei, GLboolean, const GLfloat *)
        mtgldebug_execute(glUniformMatrix4fv, location, count, transpose, value);
}

#pragma mark - APPLE

#ifdef MTGLDEBUG_PLATFORM_ANDROID

#endif

#pragma mark - APPLE

#ifdef MTGLDEBUG_PLATFORM_APPLE
/** Other **/

void mtgldebug_CFRelease(CFTypeRef object) {
    if (object) {
        const void *obj = (const void *)object; //如果不使用__unsafe_unretained会导致非OC对象强转为OC对象而导致崩溃
        Class origClass = object_getClass((id)obj);
        BOOL isCFType = strcmp(class_getName(origClass), "__NSCFType") == 0;

        // 防止使用CFRelease释放OC对象导致CFGetTypeID方法找不到而崩溃
        //    再往下走就要释放了，所以为1的时候我们先下手为强
        if (isCFType && CFGetTypeID(object) == CVOpenGLESTextureGetTypeID() && CFGetRetainCount(object) == 1) {
            MTGLDebugCore_DeleteObject((GLuint)object, MT_GLDEBUG_TEXTURE, MTGLDebugCoreObjectCVTexture);
        }
    }

    void (*originFunc)(CFTypeRef object);
    originFunc = mtgldebug_GetCFRelease();
    originFunc(object);
}

void mtgldebug_CVBufferRelease(CV_RELEASES_ARGUMENT CVBufferRef CV_NULLABLE buffer) {
    if (buffer) {
        const void *obj = (const void *)buffer; //如果不使用__unsafe_unretained会导致非OC对象强转为OC对象而导致崩溃
        Class origClass = object_getClass((id)obj);
        BOOL isCFType = strcmp(class_getName(origClass), "__NSCFType") == 0;

        // 防止使用CVBufferRelease释放OC对象导致CFGetTypeID方法找不到而崩溃
        //    再往下走就要释放了，所以为1的时候我们先下手为强
        if (isCFType && CFGetTypeID(buffer) == CVOpenGLESTextureGetTypeID() && CFGetRetainCount(buffer) == 1) {
            MTGLDebugCore_DeleteObject((GLuint)buffer, MT_GLDEBUG_TEXTURE, MTGLDebugCoreObjectCVTexture);
        }
    }

    void (*originFunc)(CV_RELEASES_ARGUMENT CVBufferRef CV_NULLABLE buffer);
    originFunc = mtgldebug_GetCVBufferRelease();
    originFunc(buffer);
}

CVReturn mtgldebug_CVOpenGLESTextureCacheCreateTextureFromImage(CFAllocatorRef CV_NULLABLE allocator,
    CVOpenGLESTextureCacheRef CV_NONNULL textureCache,
    CVImageBufferRef CV_NONNULL sourceImage,
    CFDictionaryRef CV_NULLABLE textureAttributes,
    GLenum target,
    GLint internalFormat,
    GLsizei width,
    GLsizei height,
    GLenum format,
    GLenum type,
    size_t planeIndex,
    CV_RETURNS_RETAINED_PARAMETER CVOpenGLESTextureRef CV_NULLABLE *CV_NONNULL textureOut) {

    CVReturn (*originFunc)(CFAllocatorRef CV_NULLABLE allocator,
        CVOpenGLESTextureCacheRef CV_NONNULL textureCache,
        CVImageBufferRef CV_NONNULL sourceImage,
        CFDictionaryRef CV_NULLABLE textureAttributes,
        GLenum target,
        GLint internalFormat,
        GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type,
        size_t planeIndex,
        CV_RETURNS_RETAINED_PARAMETER CVOpenGLESTextureRef CV_NULLABLE * CV_NONNULL textureOut);
    originFunc = mtgldebug_GetCVOpenGLESTextureCacheCreateTextureFromImage();
    CVReturn err = originFunc(allocator,
        textureCache,
        sourceImage,
        textureAttributes,
        target,
        internalFormat,
        width,
        height,
        format,
        type,
        planeIndex,
        textureOut);

    if (!err) {
        GLuint childObject = CVOpenGLESTextureGetName(*textureOut);
        size_t memorySize = MTGLDebugCore_CalculateMemorySize(width, height, format, type);
        GLuint parentObject = (GLuint)*textureOut;
        MTGLDebugCore_AddCVObject(parentObject, MT_GLDEBUG_TEXTURE, memorySize, childObject, MT_GLDEBUG_TEXTURE, sizeof(GLuint));
        MTGLDebugCore_TextureOutputObjectSetSize(MT_GLDEBUG_TEXTURE, parentObject, width, height);

        CFDictionaryRef attachments = CVBufferGetAttachments(sourceImage, kCVAttachmentMode_ShouldPropagate);
        CFStringRef string = CFStringCreateWithCString(kCFAllocatorDefault,
            "MetadataDictionary",
            kCFStringEncodingUTF8);
        CFDictionaryRef metadata = CFDictionaryGetValue(attachments, string);
        CFRelease(string);
        //pixelFormat == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange/VideoRange[系统正常输出的YUV数据]
        //如果相机输出的格式是 kCVPixelFormatType_32BGRA[系统输出BRGA数据可能和FBO创建一致]
        //则使用metadata的作为相机输出CVPixelBuffer的判断依据

        MTGLDebugCore_CVOutputObjectSetIsFromCamera(MT_GLDEBUG_TEXTURE, parentObject, metadata != NULL);
    }
    return err;
}
#endif

#ifdef MTGLDEBUG_PLATFORM_ANDROID

/*
 * mEGLContext = mEgl.eglCreateContext(mEGLDisplay,    //创建的和本地窗口系统的连接
 *              mEGLConfig, // 前面选好的可用EGLConfig
 *              EGL10.EGL_NO_CONTEXT,   // 允许多个EGLContext共享特定类型的数据，传递EGL_NO_CONTEXT表示不与其他上下文共享资源
 *              attrib_list);   // 指定操作的属性列表，只能接受一个属性EGL_CONTEXT_CLIENT_VERSION用来表示使用的OpenGL ES版本
 */
EGLContext mtgldebug_eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    EGLContext result;
    /*
     * #define mtgldebug_func(M_NAME,RETURN_TYPE,...) \
     *     RETURN_TYPE (*func)(__VA_ARGS__) = mtgldebug_Get##M_NAME(); \
     *
     *     __VA_ARGS__ 代表可变参数，也就是不确定个参数
     *     ## 在宏中是连接两个单元，也就是将 mtgldebug_Get 和 M_NAME 连在一起组成一个完整的函数名
     *
     *     mtgldebug_func(eglCreateContext, EGLContext, EGLDisplay, EGLConfig, EGLContext, const EGLint *) 相当于：
     *          EGLContext (*func)(EGLDisplay, EGLConfig, EGLContext, const EGLint *) = mtgldebug_GeteglCreateContext();
     *
     *     EGLContext 表示返回类型
     *     (*func) 定义函数指针，参数类型为括号中的
     *
     */
    mtgldebug_func(eglCreateContext, EGLContext, EGLDisplay, EGLConfig, EGLContext, const EGLint *)
        //mtgldebug_GeteglCreateContext();
        //    mtgldebug_execute_return(eglCreateContext, result, dpy, config, share_context, attrib_list)
        if (MTGLDebugCore_IsEnableCheckAPIUsageStates()) {
        GLuint err = glGetError();
        if (err) {
            exception("unknown", err);
        }
        _Pragma("clang diagnostic push")
            _Pragma("clang diagnostic ignored \"-Wstrict-prototypes\"")
                result
            = func(dpy, config, share_context, attrib_list);
        err = glGetError();
        if (err) {
            exception("eglCreateContext", err);
        }
    }
    else {
        result = func(dpy, config, share_context, attrib_list);
    }
    _Pragma("clang diagnostic pop")

        MTGLDebugCore_BindContext(result, share_context, attrib_list);
    return result;
}

#endif

#pragma mark - BIND

#define mtgldebug_glFunction_register(M_NAME) ReplaceFunc(#M_NAME, &M_NAME, (void **)mtgldebug_GetAddress_##M_NAME(), (void **)&mtgldebug_##M_NAME);

/*
 * 在这里进行了Hook,把系统的函数全部自己代理了
 */
void mtglDebug_register() {
    // glfunction replace
    /*************************/

    mtgldebug_glFunction_register(glGenTextures)
        mtgldebug_glFunction_register(glBindTexture)
            mtgldebug_glFunction_register(glTexImage2D)
                mtgldebug_glFunction_register(glDeleteTextures)
                    mtgldebug_glFunction_register(glTexSubImage2D)

        /******/

        mtgldebug_glFunction_register(glGenFramebuffers)
            mtgldebug_glFunction_register(glBindFramebuffer)
                mtgldebug_glFunction_register(glFramebufferTexture2D)
                    mtgldebug_glFunction_register(glDeleteFramebuffers)
                        mtgldebug_glFunction_register(glFramebufferRenderbuffer)

        /******/

        mtgldebug_glFunction_register(glGenBuffers)
            mtgldebug_glFunction_register(glBindBuffer)
                mtgldebug_glFunction_register(glBufferData)
                    mtgldebug_glFunction_register(glBufferSubData)
                        mtgldebug_glFunction_register(glDeleteBuffers)

        /******/

        mtgldebug_glFunction_register(glGenRenderbuffers)
            mtgldebug_glFunction_register(glBindRenderbuffer)
                mtgldebug_glFunction_register(glDeleteRenderbuffers)

        /******/

        mtgldebug_glFunction_register(glCreateProgram)
            mtgldebug_glFunction_register(glLinkProgram)
                mtgldebug_glFunction_register(glUseProgram)
                    mtgldebug_glFunction_register(glDeleteProgram)

                        mtgldebug_glFunction_register(glGetAttribLocation)
                            mtgldebug_glFunction_register(glGetUniformLocation)


                                mtgldebug_glFunction_register(glEnableVertexAttribArray)
                                    mtgldebug_glFunction_register(glVertexAttribPointer)
                                        mtgldebug_glFunction_register(glUniform1i)
                                            mtgldebug_glFunction_register(glUniformMatrix4fv)

                                                mtgldebug_glFunction_register(glUniform1f)
                                                    mtgldebug_glFunction_register(glUniform2f)


        /******/

        mtgldebug_glFunction_register(glDrawElements)
            mtgldebug_glFunction_register(glDrawArrays)
                mtgldebug_glFunction_register(glClear)

    /******/

#ifdef MTGLDEBUG_PLATFORM_ANDROID
                    mtgldebug_glFunction_register(eglCreateContext);
    xhook_refresh(0);
#endif

#ifdef MTGLDEBUG_PLATFORM_APPLE
    mtgldebug_glFunction_register(CVOpenGLESTextureCacheCreateTextureFromImage)
        mtgldebug_glFunction_register(CFRelease)
            mtgldebug_glFunction_register(CVBufferRelease)
#endif

    /******/
}
