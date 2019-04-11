//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/7/17
// Created by: Zed
//


#ifndef MTGLDebugObject_Internal_h
#define MTGLDebugObject_Internal_h

#include "MTGLDebugCoreObject.hpp"
#import "MTGLDebugObject.h"
@interface MTGLDebugObject () {
    NSMutableArray<NSArray *> *_operationInfoArray;
}
/**
 C++对象转OC
 */
+ (instancetype)covertCplusplusObject:(GLDebug::MTGLDebugCoreObject *)obj;

@end

#endif /* MTGLDebugObject_Internal_h */
