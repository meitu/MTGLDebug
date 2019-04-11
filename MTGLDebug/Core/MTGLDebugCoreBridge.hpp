//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/12/28
// Created by: Zed
//


#ifndef MTGLDebugCoreBridge_hpp
#define MTGLDebugCoreBridge_hpp

#include <stdbool.h>
#include <stdio.h>
#include "MTGLDebug_GL.h"

#ifdef MTGLDEBUG_PLATFORM_ANDROID
#include <jni.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MT_GLDEBUG_BUFFER 0x1
#define MT_GLDEBUG_PROGRAM 0x2
#define MT_GLDEBUG_FRAMEBUFFER 0x3
#define MT_GLDEBUG_TEXTURE 0x4

enum MTGLDebugCoreObjectClassBridge {
    //MTGLDebugcoreObject
    MTGLDebugCoreObjectBase = 1 << 0,

    //MTGLDebugTextureCoreObject
    MTGLDebugCoreObjectTexture = 1 << 1,

#ifdef MTGLDEBUG_PLATFORM_APPLE
    //MTGLDebugCVCoreObject
    MTGLDebugCoreObjectCVTexture = 1 << 2,
#endif
    //MTGLDebugProgramCoreObject
    MTGLDebugCoreObjectProgram = 1 << 3,

    //MTGLDebugFramebufferCoreObject
    MTGLDebugCoreObjectFramebuffer = 1 << 4
};


typedef void (*APICallback)(const void *result, const char *methodName, ...);

/**
     检查当前对象是否在当前sharegroup中

     @param object 对象值
     @param target 类型
     @return 结果
     */
bool MTGLDebugCore_CheckObject(unsigned int object,
    unsigned int target,
    void *outputShareGroupObjects,
    int initClass,
    unsigned int *outputErrorType);
/**
     添加托管对象
     
     @param object 对象值
     @param target 类型
     @param memorySize 内存占用
     @param initClass 用于比对的类对象
     @param outputObject 添加成功后的对象
     */
void MTGLDebugCore_AddObject(unsigned int object,
    unsigned int target,
    size_t memorySize,
    int initClass,
    void **outputObject);

#ifdef MTGLDEBUG_PLATFORM_APPLE
void MTGLDebugCore_AddCVObject(unsigned int pObject, unsigned int pTarget, size_t pMemorySize,
    unsigned int cObject, unsigned int cTarget, size_t cMemorySize);
#endif
/**
     移除托管对象
     
     @param object 对象值
     @param target 类型
     @param initClass 用于比对的类对象
     @return 结果
     */
bool MTGLDebugCore_DeleteObject(unsigned int object, unsigned int target, int initClass);

void MTGLDebugCore_OutputObjectAppendMemorySize(GLenum target, unsigned int object, size_t memSize);
void MTGLDebugCore_DeleteObjects(int count, const unsigned int *params, unsigned int target, int initClass);

bool MTGLDebugCore_IsEnableGLDebugException(void);
bool MTGLDebugCore_IsEnableGLDebug(void);

void MTGLDebugCore_DidDraw(void);

/**
	 设置GL对象的相关属性
	 */
void MTGLDebugCore_TextureOutputObjectSetSize(GLenum target, unsigned int object, GLsizei width, GLsizei height);
void MTGLDebugCore_OutputObjectSetMemorySize(GLenum target, unsigned int object, size_t memSize);
void MTGLDebugCore_ProgramObjectSetShaderString(GLenum taget, unsigned int object, int shaderType, char *str, int length);



size_t MTGLDebugCore_CalculateMemorySize(GLsizei width, GLsizei height, GLenum format, GLenum type);


/**
     是否开启API正确性检测

     @return 结果
     */
bool MTGLDebugCore_IsEnableCheckAPIUsageStates(void);

/**
     抛出GL异常函数

     @param methodName GL函数
     @param error 错误类型
     */
void MTGLDebugCore_Excption(const char *methodName, GLenum error);

APICallback MTGLDebugCore_WillCallAPICallback(void);
APICallback MTGLDebugCore_DidCallAPICallback(void);

#ifdef MTGLDEBUG_PLATFORM_APPLE
void MTGLDebugCore_CVOutputObjectSetIsFromCamera(GLenum target, unsigned int object, bool isFromCamera);
void MTGLDebugCore_CVOutputObjectSetChild(GLenum target, unsigned int parentObject, unsigned int childObject);
#endif
void MTGLDebugCore_FramebufferObjectSetTexture(GLenum target, unsigned int object, unsigned int texture);


#ifdef MTGLDEBUG_PLATFORM_ANDROID

void MTGLDebugCore_BindContext(EGLContext context, EGLContext shareContext, const EGLint *attrib);

JNIEXPORT jboolean JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_IsEnable(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIStackInformation(JNIEnv *env, jobject obj, jboolean enable);
JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIDrawCallsPerSeconds(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNISortObjectsByMemory(JNIEnv *env, jobject obj);

JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIStackInformation(JNIEnv *env, jobject obj, jboolean enable);
JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIDrawCallsPerSeconds(JNIEnv *env, jobject obj);
JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNISortObjectsByMemory(JNIEnv *env, jobject obj);

#endif


#ifdef __cplusplus
}
#endif

#endif /* MTGLDebugCoreBridge_hpp */
