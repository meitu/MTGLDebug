//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/7/12
// Created by: wuzeqin
//


#import <UIKit/UIKit.h>
#import "MTGLDebugGLFunction.h"
#import "MTGLDebugImpl_Internal.h"
#import "MTGLDebugObject+QuickLook.h"
@implementation MTGLDebugObject (QuickLook)

- (id)debugQuickLookObject {
    if ([self target] == 0x3) {
        UIImage *frambufferImage = nil; //[self framebufferContent];
        if (frambufferImage) {
            return frambufferImage;
        } else {
            return @"framebuffer快照创建失败";
        }

    } else if ([self target] == 0x4) {
        MTGLDebugTextureObject *textureObject = (MTGLDebugTextureObject *)self;
        CGSize textureSize = CGSizeMake(textureObject.width, textureObject.height);
        if (CGSizeEqualToSize(CGSizeZero, textureSize)) {
            return @"当前纹理大小为0,创建失败";
        } else {
            return [self textureContent:textureSize];
        }
    } else {
        return @"当前对象不支持快照";
    }
}

void __mtglDebugObjectFramebufferPixelDataRelease(void *info, const void *data, size_t size);

inline void __mtglDebugObjectFramebufferPixelDataRelease(void *info, const void *data, size_t size) {
    free((void *)data);
}


- (UIImage *)textureContent:(CGSize)textureSize {
    EAGLContext *originContext = [EAGLContext currentContext];
    [EAGLContext setCurrentContext:self.context];

    //获取当前framebuffer原始
    GLint originFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originFramebuffer);
    GLuint tempFramebuffer;
    void (*origin_glGenFramebuffers)(GLsizei n, GLuint * framebuffers);
    origin_glGenFramebuffers = mtgldebug_GetglGenFramebuffers();
    origin_glGenFramebuffers(1, &tempFramebuffer);

    void (*origin_glBindFramebuffer)(GLenum target, GLuint framebuffer);
    origin_glBindFramebuffer = mtgldebug_GetglBindFramebuffer();
    origin_glBindFramebuffer(GL_FRAMEBUFFER, tempFramebuffer);

    void (*origin_glFramebufferTexture2D)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    origin_glFramebufferTexture2D = mtgldebug_GetglFramebufferTexture2D();
    origin_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self.object, 0);

    CGSize framebufferSize = CGSizeMake(textureSize.width, textureSize.height);
    size_t totalBytesForImage = framebufferSize.width * framebufferSize.height * 4;
    GLubyte *bytes = malloc(sizeof(GLubyte) * totalBytesForImage);
    glReadPixels(0, 0, framebufferSize.width, framebufferSize.height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData((__bridge void *)(self), bytes, totalBytesForImage, __mtglDebugObjectFramebufferPixelDataRelease);
    CGColorSpaceRef defaultRGBColorSpace = CGColorSpaceCreateDeviceRGB();
    // 创建CGImage
    CGImageRef cgImageFromBytes = CGImageCreate(framebufferSize.width, framebufferSize.height, 8, 32, 4 * framebufferSize.width, defaultRGBColorSpace, kCGBitmapByteOrderDefault, dataProvider, NULL, NO, kCGRenderingIntentDefault);
    // 释放无用数据
    CGDataProviderRelease(dataProvider);
    CGColorSpaceRelease(defaultRGBColorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImageFromBytes];
    CGImageRelease(cgImageFromBytes);

    void (*origin_glDeleteFramebuffers)(GLsizei n, const GLuint *framebuffers);
    origin_glDeleteFramebuffers = mtgldebug_GetglDeleteFramebuffers();
    origin_glDeleteFramebuffers(1, &tempFramebuffer);

    [EAGLContext setCurrentContext:originContext];
    origin_glBindFramebuffer(GL_FRAMEBUFFER, originFramebuffer);


    return image;
}

- (UIImage *)framebufferContent {
    EAGLContext *originContext = [EAGLContext currentContext];
    [EAGLContext setCurrentContext:self.context];
    //获取当前framebuffer原始
    GLint originFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originFramebuffer);

    void (*originFunc_glBindFramebuffer)(GLenum target, GLuint framebuffer);
    originFunc_glBindFramebuffer = mtgldebug_GetglBindFramebuffer();
    originFunc_glBindFramebuffer(GL_FRAMEBUFFER, self.object);

    GLint textureID;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &textureID);

    MTGLDebugTextureObject *debugObject = (MTGLDebugTextureObject *)[MTGLDebug debugObjectForContext:self.context
                                                                                              target:0x4
                                                                                              object:textureID];

    CGSize framebufferSize = CGSizeMake(debugObject.width, debugObject.height);
    size_t totalBytesForImage = framebufferSize.width * framebufferSize.height * 4;
    GLubyte *bytes = malloc(sizeof(GLubyte) * totalBytesForImage);
    glReadPixels(0, 0, framebufferSize.width, framebufferSize.height, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData((__bridge void *)(self), bytes, totalBytesForImage, __mtglDebugObjectFramebufferPixelDataRelease);
    CGColorSpaceRef defaultRGBColorSpace = CGColorSpaceCreateDeviceRGB();
    // 创建CGImage
    CGImageRef cgImageFromBytes = CGImageCreate(framebufferSize.width, framebufferSize.height, 8, 32, 4 * framebufferSize.width, defaultRGBColorSpace, kCGBitmapByteOrderDefault, dataProvider, NULL, NO, kCGRenderingIntentDefault);
    // 释放无用数据
    CGDataProviderRelease(dataProvider);
    CGColorSpaceRelease(defaultRGBColorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImageFromBytes];
    CGImageRelease(cgImageFromBytes);
    // 还原原始framebuffer
    [EAGLContext setCurrentContext:originContext];
    originFunc_glBindFramebuffer(GL_FRAMEBUFFER, originFramebuffer);
    return image;
}

@end
