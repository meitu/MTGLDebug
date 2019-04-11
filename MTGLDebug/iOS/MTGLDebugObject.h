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


#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>


@interface MTGLDebugObject : NSObject

@property (nonatomic, readonly, assign) GLenum target;

@property (nonatomic, readonly, assign) GLuint object;

@property (nonatomic, readonly, unsafe_unretained) EAGLContext *context;

@property (nonatomic, readonly, assign) size_t memorySize;

@property (nonatomic, readonly, unsafe_unretained) EAGLSharegroup *sharegroup;

@property (nonatomic, readonly, copy) NSString *timestamp;

@property (nonatomic, readonly, assign) double timestampInDouble;

@property (nonatomic, readonly, copy) NSArray<NSArray<NSString *> *> *operationInfoArray;

- (NSString *)targetString;

@end


@interface MTGLDebugTextureObject : MTGLDebugObject

@property (nonatomic, assign) GLsizei width;
@property (nonatomic, assign) GLsizei height;

@end

@interface MTGLDebugCVObject : MTGLDebugTextureObject

@property (nonatomic, weak, readonly) MTGLDebugObject *child;

/**
 是否是camera数据流产生的纹理
 */
@property (nonatomic, assign, readonly) BOOL isFromCamera;

@end

typedef MTGLDebugTextureObject MTGLDebugRenderbufferObject;

@interface MTGLDebugProgramObject : MTGLDebugObject

@property (nonatomic, strong) NSString *vertexShader;
@property (nonatomic, strong) NSString *fragmentShader;

@end
