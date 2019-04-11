//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/12/25
// Created by: Zed
//


#import <OpenGLES/EAGL.h>

#ifdef __cplusplus
extern "C" {
#endif
bool currentContextIsExist() {
    return [EAGLContext currentContext] != NULL;
}
#ifdef __cplusplus
}
#endif
