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


#ifndef MTGLDebugGLFunction_h
#define MTGLDebugGLFunction_h

#include <stdio.h>
#include "MTGLDebug_GL.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE
#include <CoreVideo/CoreVideo.h>
#include <OpenGLES/gltypes.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
#define mtgldebug_glFunction_define(M_NAME) \
    void *mtgldebug_Get##M_NAME(void);      \
    void **mtgldebug_GetAddress_##M_NAME(void);

// origin function interface
/*************************/

mtgldebug_glFunction_define(glGenTextures)
mtgldebug_glFunction_define(glBindTexture)
mtgldebug_glFunction_define(glTexImage2D)
mtgldebug_glFunction_define(glTexSubImage2D)
mtgldebug_glFunction_define(glDeleteTextures)

/******/

mtgldebug_glFunction_define(glGenFramebuffers)
mtgldebug_glFunction_define(glBindFramebuffer)
mtgldebug_glFunction_define(glFramebufferTexture2D)
mtgldebug_glFunction_define(glDeleteFramebuffers)
mtgldebug_glFunction_define(glFramebufferRenderbuffer)

/******/

mtgldebug_glFunction_define(glGenBuffers)
mtgldebug_glFunction_define(glBindBuffer)
mtgldebug_glFunction_define(glBufferData)
mtgldebug_glFunction_define(glBufferSubData)
mtgldebug_glFunction_define(glDeleteBuffers)

/******/

mtgldebug_glFunction_define(glGenRenderbuffers)
mtgldebug_glFunction_define(glBindRenderbuffer)
mtgldebug_glFunction_define(glDeleteRenderbuffers)

/******/

mtgldebug_glFunction_define(glLinkProgram)
mtgldebug_glFunction_define(glUseProgram)
mtgldebug_glFunction_define(glDeleteProgram)
mtgldebug_glFunction_define(glCreateProgram)
mtgldebug_glFunction_define(glEnableVertexAttribArray)
mtgldebug_glFunction_define(glVertexAttribPointer)
mtgldebug_glFunction_define(glGetAttribLocation)
mtgldebug_glFunction_define(glGetUniformLocation)
mtgldebug_glFunction_define(glUniform1i)
mtgldebug_glFunction_define(glUniform1f)
mtgldebug_glFunction_define(glUniform2f)
mtgldebug_glFunction_define(glUniformMatrix4fv)

/******/
mtgldebug_glFunction_define(glDrawArrays)
mtgldebug_glFunction_define(glDrawElements)
mtgldebug_glFunction_define(glClear)

/******/

#ifdef MTGLDEBUG_PLATFORM_APPLE
mtgldebug_glFunction_define(CVOpenGLESTextureCacheCreateTextureFromImage)
mtgldebug_glFunction_define(CVBufferRelease)
mtgldebug_glFunction_define(CFRelease)
#endif

#ifdef MTGLDEBUG_PLATFORM_ANDROID
mtgldebug_glFunction_define(eglCreateContext);
#endif

/******/

// clang-format on

/*************************/


// hook replaceFunction
void ReplaceFunc(const char *funcName,
    const void *originFuncAddress,
    void **originFunc,
    void **replaceFunc);

#ifdef __cplusplus
}
#endif

#endif /* MTGLDebugGLFunction_h */
