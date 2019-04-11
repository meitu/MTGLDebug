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


#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
void mtGLDebugAutoreleasePoolPush(void **outputAutoreleasPool);
void mtGLDebugautoreleasePoolPop(void *autoreleasPool);
#ifdef __cplusplus
}
#endif
