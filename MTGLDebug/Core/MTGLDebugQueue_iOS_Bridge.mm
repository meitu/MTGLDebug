//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/7/27
// Created by: Zed
//


#import "MTGLDebugQueue_iOS_Bridge.h"
#include <Foundation/Foundation.h>

void mtGLDebugAutoreleasePoolPush(void **outputAutoreleasPool) {
    NSAutoreleasePool *autoreleasePool = [[NSAutoreleasePool alloc] init];
    if (outputAutoreleasPool) {
        *outputAutoreleasPool = (__bridge void *)autoreleasePool;
    } else {
        [autoreleasePool release];
    }
}

void mtGLDebugautoreleasePoolPop(void *autoreleasPool) {
    if (autoreleasPool) {
        NSAutoreleasePool *pool = (__bridge id)autoreleasPool;
        [pool release];
    }
}
