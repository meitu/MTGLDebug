//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2016/12/22
// Created by: wuzeqin
//


#ifndef MTGLHeader_Internal_h
#define MTGLHeader_Internal_h

#import <objc/runtime.h>
#import "MTGLDebugGLFunction.h"
#import "MTGLDebugImpl.h"
#import "MTGLDebugObject.h"

@interface MTGLDebug ()

#ifdef UNITTEST
/**
 设置是否开启或关闭MTGLDebug异常错误抛出,内部单元测试自己使用
 */
@property (nonatomic, assign, class, getter=isEnableGLDebugException) BOOL enableGLDebugException;
#endif

+ (MTGLDebugObject *)debugObjectForContext:(EAGLContext *)context
                                    target:(GLenum)target
                                    object:(GLuint)objectId;


+ (void)didDraw;

@end


#endif /* MTGLHeader_Internal_h */
