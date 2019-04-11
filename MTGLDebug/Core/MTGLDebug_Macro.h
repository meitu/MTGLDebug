//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/1/5
// Created by: Zed
//


#ifndef MTGLDebug_Macro_h
#define MTGLDebug_Macro_h

#include <stdbool.h>
#include "MTGLDebug_Platform.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE

#ifndef DEBUG
#define MTGLDEBUG_IS_RELEASE 1
#else
#define MTGLDEBUG_IS_DEBUG 1
#endif

#else

#ifdef NDEBUG
#define MTGLDEBUG_IS_RELEASE 1
#else
#define MTGLDEBUG_IS_DEBUG 1
#endif

#endif


#ifdef MTGLDEBUG_PLATFORM_ANDROID

#include <android/log.h>
#ifndef LOG_TAG
#define LOG_TAG "GLDebug"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, ##__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##__VA_ARGS__)  // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, ##__VA_ARGS__)  // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, ##__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, ##__VA_ARGS__) // 定义LOGF类型
#define MTGLDEBUG_unsafe_printf(format, ...) \
        LOGD(format, ##__VA_ARGS__);

#endif

#define MTGLDEBUG_ANDROID(x) x

#else

#define MTGLDEBUG_unsafe_printf(format, ...) \
        printf(format, ##__VA_ARGS__);       \
        printf("\n");

#define MTGLDEBUG_ANDROID(x)

#endif


#ifdef MTGLDEBUG_PLATFORM_APPLE

#define MTGLDEBUG_APPLE(x) x

#else

#define MTGLDEBUG_APPLE(x)

#endif


#ifdef MTGLDEBUG_IS_RELEASE
#define MTGLDEBUG_printf(format, ...)
#define MTGLDEBUG_assert(CONDITION, format, ...)
#else
#include <assert.h>

#ifdef MTGLDEBUG_PLATFORM_ANDROID
#define MTGLDEBUG_printf(format, ...) LOGD(format, ##__VA_ARGS__);
#define MTGLDEBUG_assert(CONDITION, format, ...) \
    if (!(CONDITION)) {                          \
        LOGE(format, ##__VA_ARGS__);             \
        assert(false);                           \
    }


#else
#define MTGLDEBUG_printf(format, ...) \
    printf(format, ##__VA_ARGS__);    \
    printf("\n")
#define MTGLDEBUG_assert(CONDITION, format, ...) \
    if (!(CONDITION)) {                          \
        MTGLDEBUG_printf(format, ##__VA_ARGS__); \
        assert(false);                           \
    }
#endif

#endif

#define MTGLDEBUG_parameterAssert(CONDITION) MTGLDEBUG_assert(CONDITION, "Invalid parameter not satisfying: %s", #CONDITION);


#endif /* MTGLDebug_Macro_h */
