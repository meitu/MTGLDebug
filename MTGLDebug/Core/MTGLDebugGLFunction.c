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


#include "MTGLDebugGLFunction.h"
#include <assert.h>
#include <stdbool.h>
#include "MTGLDebug_Platform.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE
#include <dlfcn.h>
#import <fishhook/fishhook.h>
#else
#include "xhook.h"
#endif

#define mtgldebug_glFunction_implementer(M_NAME) \
    static void *orig_##M_NAME;                  \
    void *mtgldebug_Get##M_NAME(void) {          \
        return orig_##M_NAME;                    \
    }                                            \
    void **mtgldebug_GetAddress_##M_NAME(void) { \
        return &orig_##M_NAME;                   \
    }

// origin function implement
/*************************/

mtgldebug_glFunction_implementer(glGenTextures)
    mtgldebug_glFunction_implementer(glBindTexture)
        mtgldebug_glFunction_implementer(glTexImage2D)
            mtgldebug_glFunction_implementer(glTexSubImage2D)
                mtgldebug_glFunction_implementer(glDeleteTextures)

    /******/

    mtgldebug_glFunction_implementer(glGenFramebuffers)
        mtgldebug_glFunction_implementer(glBindFramebuffer)
            mtgldebug_glFunction_implementer(glFramebufferTexture2D)
                mtgldebug_glFunction_implementer(glDeleteFramebuffers)
                    mtgldebug_glFunction_implementer(glFramebufferRenderbuffer)

    /******/

    mtgldebug_glFunction_implementer(glGenBuffers)
        mtgldebug_glFunction_implementer(glBindBuffer)
            mtgldebug_glFunction_implementer(glBufferData)
                mtgldebug_glFunction_implementer(glBufferSubData)
                    mtgldebug_glFunction_implementer(glDeleteBuffers)

    /******/

    mtgldebug_glFunction_implementer(glGenRenderbuffers)
        mtgldebug_glFunction_implementer(glBindRenderbuffer)
            mtgldebug_glFunction_implementer(glDeleteRenderbuffers)

    /******/

    mtgldebug_glFunction_implementer(glLinkProgram)
        mtgldebug_glFunction_implementer(glUseProgram)
            mtgldebug_glFunction_implementer(glDeleteProgram)
                mtgldebug_glFunction_implementer(glCreateProgram)
                    mtgldebug_glFunction_implementer(glEnableVertexAttribArray)
                        mtgldebug_glFunction_implementer(glVertexAttribPointer)
                            mtgldebug_glFunction_implementer(glGetAttribLocation)
                                mtgldebug_glFunction_implementer(glGetUniformLocation)
                                    mtgldebug_glFunction_implementer(glUniform1i)

                                        mtgldebug_glFunction_implementer(glUniform2f)
                                            mtgldebug_glFunction_implementer(glUniform1f)

                                                mtgldebug_glFunction_implementer(glUniformMatrix4fv)

    /******/

    mtgldebug_glFunction_implementer(glDrawArrays)
        mtgldebug_glFunction_implementer(glDrawElements)
            mtgldebug_glFunction_implementer(glClear)

/******/

#ifdef MTGLDEBUG_PLATFORM_APPLE
                mtgldebug_glFunction_implementer(CVOpenGLESTextureCacheCreateTextureFromImage)
                    mtgldebug_glFunction_implementer(CVBufferRelease)
                        mtgldebug_glFunction_implementer(CFRelease)
#endif

#ifdef MTGLDEBUG_PLATFORM_ANDROID
                            mtgldebug_glFunction_implementer(eglCreateContext);
#endif

// releaceFunc
void ReplaceFunc(const char *funcName, const void *originFuncAddress, void **originFunc, void **replaceFunc) {

#ifdef MTGLDEBUG_PLATFORM_APPLE
    *originFunc = (void *)dlsym(RTLD_DEFAULT, funcName);
    rebind_symbols((struct rebinding[]){funcName, replaceFunc}, 1);
#else


    //    if (registerInlineHook((uint32_t) originFuncAddress, (uint32_t)replaceFunc, (uint32_t **)originFunc) != ELE7EN_OK) {
    //        assert(false);
    //    }
    //    if (inlineHook((uint32_t)originFuncAddress) != ELE7EN_OK) {
    //        assert(false);
    //    }

    if (xhook_register(".*\\.so$", funcName, replaceFunc, originFunc) != 0) {
        assert(false);
    }
#endif
}
