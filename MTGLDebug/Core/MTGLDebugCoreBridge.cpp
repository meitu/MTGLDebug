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


#include "MTGLDebugCoreBridge.hpp"
#include <map>
#include "MTGLDebugCore.hpp"
#include "MTGLDebug_Macro.h"
#ifdef MTGLDEBUG_PLATFORM_ANDROID
#include "../Android/MTGLDebugCore_Android.h"
#include "MTGLDebugGLHookFunction.h"
#endif

using namespace GLDebug;

bool MTGLDebugCore_CheckObject(unsigned int object, unsigned int target, void *outputShareGroupObjects,
    int initClass, unsigned int *outputErrorType) {

    MTGLDebugCoreInputObject inputObject;
    inputObject.object = object;
    inputObject.target = target;
    inputObject.initClass = initClass;
    return MTGLDebugCore::checkObjectPublic(inputObject, (MTGLDebugCoreObjectMap **)outputShareGroupObjects, outputErrorType);
}

void MTGLDebugCore_DidDraw() {
    MTGLDebugCore::didDrawPublic();
}

void MTGLDebugCore_OutputObjectAppendMemorySize(GLenum target, unsigned int object, size_t memSize) {

    const void *currentContextSharegourp = GLDebug::MTGLDebugCore::currentContextSharegourpPublic();
    GLDebug::MTGLDebugCore::runSyncBlockInTheSafeThread([&] {
        MTGLDebugCoreObject *outputObject = GLDebug::MTGLDebugCore::fetchDebugObjectPublic(currentContextSharegourp, target, object);
        if (!outputObject) {
            return;
        }
        ((MTGLDebugCoreObject *)outputObject)->appendSize(memSize);
    });
}

void MTGLDebugCore_DeleteObjects(int count, const unsigned int *params, unsigned int target, int initClass) {
    MTGLDebugCore::deleteObjectsPublic(count, params, target, initClass);
}

bool MTGLDebugCore_DeleteObject(unsigned int object, unsigned int target, int initClass) {
    MTGLDebugCoreInputObject inputObject;
    inputObject.object = object;
    inputObject.target = target;
    inputObject.initClass = initClass;
    return MTGLDebugCore::deleteObjectPublic(inputObject);
}

bool MTGLDebugCore_IsEnableGLDebugException() {
    return MTGLDebugCore::isEnableGLDebugException();
}



bool MTGLDebugCore_IsEnableGLDebug() {
    return MTGLDebugCore::isEnable();
}

void MTGLDebugCore_AddObject(unsigned int object, unsigned int target, size_t memorySize, const int initClass, void **outputObject) {
    MTGLDebugCoreInputObject inputObject;
    inputObject.object = object;
    inputObject.target = target;
    inputObject.initClass = initClass;
    MTGLDebugCore::addObjectPublic(inputObject, memorySize);
}

#ifdef MTGLDEBUG_PLATFORM_APPLE
void MTGLDebugCore_AddCVObject(unsigned int pObject, unsigned int pTarget, size_t pMemorySize,
    unsigned int cObject, unsigned int cTarget, size_t cMemorySize) {
    GLDebug::MTGLDebugCoreInputObject parentInputObject;
    parentInputObject.object = pObject;
    parentInputObject.target = pTarget;
    parentInputObject.initClass = MTGLDebugCoreObjectCVTexture;
    GLDebug::MTGLDebugCoreInputObject childInputObject;
    childInputObject.object = cObject;
    childInputObject.target = cTarget;
    childInputObject.initClass = MTGLDebugCoreObjectTexture;
    MTGLDebugCore::addCVObjectPublic(parentInputObject, pMemorySize, childInputObject, cMemorySize);
}
#endif

size_t MTGLDebugCore_CalculateMemorySize(GLsizei width, GLsizei height, GLenum format, GLenum type) {
    return MTGLDebugCoreObject::calculateMemorySize(width, height, format, type);
}

void MTGLDebugCore_OperateFetchedDebugObject(GLenum target, unsigned int object, MTGLDebugCoreObjectClass objClass,
    std::function<void(MTGLDebugCoreObject *)> block) {
    if (object == 0) {
        //重置状态绑定0不做获取对象操作
        return;
    }

    const void *currentContextSharegroup = GLDebug::MTGLDebugCore::currentContextSharegourpPublic();
    GLDebug::MTGLDebugCore::runSyncBlockInTheSafeThread([&currentContextSharegroup, block, target, object, objClass] {
        MTGLDebugCoreObject *outputObject = GLDebug::MTGLDebugCore::fetchDebugObjectPublic(currentContextSharegroup, target, object);
        if (!outputObject) {
            if (MTGLDebugCore::isEnableGLDebugException()) {
                MTGLDEBUG_assert(false, "fetch DebugObject Failed When setting object property!!!");
            }
            return;
        }

        if (!(((MTGLDebugCoreObject *)outputObject)->classType() & objClass)) {
            if (MTGLDebugCore::isEnableGLDebugException()) {
                MTGLDEBUG_assert(false, "fetch Wrong DebugObject When setting object property!!!");
            }
            return;
        }
        block(outputObject);
    });
}

void MTGLDebugCore_TextureOutputObjectSetSize(GLenum target, unsigned int object, GLsizei width, GLsizei height) {
#ifdef MTGLDEBUG_PLATFORM_APPLE
    MTGLDebugCoreObjectClass objClass = (MTGLDebugCoreObjectClass)(MTGLDebugCoreObjectTextureClass | MTGLDebugCoreObjectCVTextureClass);
#else
    MTGLDebugCoreObjectClass objClass = MTGLDebugCoreObjectTextureClass;
#endif
    MTGLDebugCore_OperateFetchedDebugObject(target, object, objClass, [=](MTGLDebugCoreObject *outputObject) {
        MTGLDebugTextureCoreObject *textureObject = (MTGLDebugTextureCoreObject *)outputObject;
        textureObject->setWidth(width);
        textureObject->setHeight(height);
    });
}

void MTGLDebugCore_OutputObjectSetMemorySize(GLenum target, unsigned int object, size_t memSize) {
#ifdef MTGLDEBUG_PLATFORM_APPLE
    MTGLDebugCoreObjectClass objClass = (MTGLDebugCoreObjectClass)(MTGLDebugCoreObjectTextureClass | MTGLDebugCoreObjectCVTextureClass);
#else
    MTGLDebugCoreObjectClass objClass = MTGLDebugCoreObjectTextureClass;
#endif
    MTGLDebugCore_OperateFetchedDebugObject(target, object, objClass, [=](MTGLDebugCoreObject *outputObject) {
        MTGLDebugTextureCoreObject *textureObject = (MTGLDebugTextureCoreObject *)outputObject;
        textureObject->SetMemorySize(memSize);
    });
}

void MTGLDebugCore_ProgramObjectSetShaderString(GLenum taget, unsigned int object, int shaderType, char *str, int length) {
    MTGLDebugCore_OperateFetchedDebugObject(taget, object, MTGLDebugProgramCoreObjectClass, [=](MTGLDebugCoreObject *outputObject) {
        MTGLDebugProgramCoreObject *programObject = (MTGLDebugProgramCoreObject *)outputObject;

        if (shaderType == GL_VERTEX_SHADER) {
            programObject->setVertexShader(str);
        } else if (shaderType == GL_FRAGMENT_SHADER) {
            programObject->setFragmentShader(str);
        } else {
            MTGLDEBUG_assert(false, "获取脚本类型出错");
        }
    });
}


void MTGLDebugCore_FramebufferObjectSetTexture(GLenum target, unsigned int object, unsigned int texture) {

    MTGLDEBUG_assert(texture, "当前Framebuffer绑定的纹理为空");
    MTGLDebugCore_OperateFetchedDebugObject(target, object, MTGLDebugFramebuffeCoreObjectClass, [=](MTGLDebugCoreObject *outputObject) {
        MTGLDebugFramebuffeCoreObject *fboObj = (MTGLDebugFramebuffeCoreObject *)outputObject;
        fboObj->setTexture(texture);
    });
}


bool MTGLDebugCore_IsEnableCheckAPIUsageStates() {
    return MTGLDebugCore::isEnableCheckAPIUsageStates();
}

void MTGLDebugCore_Excption(const char *methodName, GLenum error) {
    MTGLDebugCore::exceptionPublic(methodName, error);
}


#pragma mark - iOS Method

#ifdef MTGLDEBUG_PLATFORM_APPLE



bool MTGLDebugCore_OutputObjectIsCVObject(void *outputObject) {
    return ((MTGLDebugCoreObject *)outputObject)->classType() == MTGLDebugCoreObjectCVTextureClass;
}

void MTGLDebugCore_CVOutputObjectSetIsFromCamera(GLenum target, unsigned int object, bool isFromCamera) {

    MTGLDebugCore_OperateFetchedDebugObject(target, object, MTGLDebugCoreObjectCVTextureClass, [=](MTGLDebugCoreObject *outputObject) {
        MTGLDebugCVCoreObject *cvObject = (MTGLDebugCVCoreObject *)outputObject;
        cvObject->isFromCamera = isFromCamera;
    });
}

void MTGLDebugCore_CVOutputObjectSetChild(GLenum target, unsigned int parentObject, unsigned int childObject) {
    MTGLDebugCVCoreObject *cvpObject = NULL;
    MTGLDebugCore_OperateFetchedDebugObject(target, parentObject, MTGLDebugCoreObjectCVTextureClass, [&](MTGLDebugCoreObject *pOutputObject) {
        cvpObject = (MTGLDebugCVCoreObject *)pOutputObject;
    });

    MTGLDebugCore_OperateFetchedDebugObject(target, childObject, MTGLDebugCoreObjectTextureClass, [=](MTGLDebugCoreObject *cOutputObjcet) {
        cvpObject->setChild((MTGLDebugTextureCoreObject *)cOutputObjcet);
    });
}

#endif

#pragma mark - JNI Method

#ifdef MTGLDEBUG_PLATFORM_ANDROID

void MTGLDebugCore_BindContext(EGLContext context, EGLContext shareContext, const EGLint *attrib) {
    ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->bindContext(context, shareContext, attrib);
}

JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIStackInformation(JNIEnv *env, jobject obj, jboolean enable) {
    MTGLDebugCoreOptions options = enable ? MTGLDebugCore::defaultDebugOptions() : MTGLDebugCoreOptionsDisable;

    MTGLDebugCore::initSharedGLDebugObject(env); //创建MTGLDebugCore_Android类对象

    MTGLDebugCore::SetDebugOptions(options);
    MTGLDebugCore_Android *debugObject_Android = (MTGLDebugCore_Android *)MTGLDebugCore_Android::sharedMTGLDebugCore(); //返回刚刚创建的MTGLDebugCore_Android对象指针

    // NewGlobalRef创建全局引用，可以跨方法、跨线程使用这种对象如不主动释放，它永远都不会被垃圾回收，必须调用DeleteGlobalRef手动释放
    debugObject_Android->javaDedebugObj = env->NewGlobalRef(obj);
    debugObject_Android->currentStacktraceCallback = [=]() {
        JNIEnv *env_new = MTGLDebugCore_Android::getENVForJVM(debugObject_Android->mGLDebugQueueJavaVM); //获取当前线程的JNIEnv指针

        //      jclass javaMovieWriterClass = env_new->GetObjectClass(debugObject_Android->javaDedebugObj);
        //      jmethodID mid = env_new->GetStaticMethodID(javaMovieWriterClass, "throwJNIException","()V");//
        //      env_new->CallStaticVoidMethod(javaMovieWriterClass, mid);

        jstring throw_str = env_new->NewStringUTF("MTGLDebugException");

        //查找系统类 Throwable
        jclass throwable_class = env_new->FindClass("java/lang/Throwable");
        //获取 Throwable初始化ID
        jmethodID throwable_init = env_new->GetMethodID(throwable_class, "<init>", "(Ljava/lang/String;)V");

        jobject throwable_obj = env_new->NewObject(throwable_class, throwable_init, throw_str);


        //查找系统类 StringWriter
        jclass stringWriter_class = env_new->FindClass("java/io/StringWriter");
        jmethodID stringWriter_init = env_new->GetMethodID(stringWriter_class, "<init>", "()V");
        jobject stringWriter_obj = env_new->NewObject(stringWriter_class, stringWriter_init);


        //查找系统类 PrinterWriter
        jclass printerWriter_class = env_new->FindClass("java/io/PrintWriter");
        jmethodID printerWriter_init = env_new->GetMethodID(printerWriter_class, "<init>", "(Ljava/io/Writer;Z)V");
        jobject printerWriter_obj = env_new->NewObject(printerWriter_class, printerWriter_init, stringWriter_obj, true);

        //调用printStackTrace
        jmethodID throwable_mid = env_new->GetMethodID(throwable_class, "printStackTrace", "(Ljava/io/PrintWriter;)V");
        env_new->CallVoidMethod(throwable_obj, throwable_mid, printerWriter_obj);

        if (throw_str != NULL) {
            env_new->DeleteLocalRef(throw_str);
        }

        //      jmethodID throwable_mid = env_new->GetMethodID(throwable_class, "printStackTrace", "()V");
        //      env_new->CallVoidMethod(throwable_obj, throwable_mid);

        //调用StringWriter的getBuffer()方法获取字符串
        jmethodID getBuffer_mid = env_new->GetMethodID(stringWriter_class, "getBuffer", "()Ljava/lang/StringBuffer;");
        jobject stringBufferObj = env_new->CallObjectMethod(stringWriter_obj, getBuffer_mid);

        jclass stringBufferClass = env_new->FindClass("java/lang/StringBuffer");
        jmethodID toString_mid = env_new->GetMethodID(stringBufferClass, "toString", "()Ljava/lang/String;");
        jstring stringObj = (jstring)env_new->CallObjectMethod(stringBufferObj, toString_mid);

        jboolean isCopy = JNI_TRUE;

        (*env_new).DeleteLocalRef(throwable_class);
        (*env_new).DeleteLocalRef(stringWriter_class);
        (*env_new).DeleteLocalRef(printerWriter_class);
        (*env_new).DeleteLocalRef(stringBufferClass);

        const char *str;
        str = env_new->GetStringUTFChars(stringObj, &isCopy);
        if (str != NULL) {
            std::string result = str;
            env_new->ReleaseStringUTFChars(stringObj, str);

#ifdef MTGLDEBUG_PLATFORM_ANDROID
            LOGE("MTGLDebugCoreBridge.cpp =====>:%s", result.c_str());
#endif
            return result;
        }
    };

    debugObject_Android->errorExceptionCallback = [=](const std::string &errorStr) {
        JNIEnv *env_new = debugObject_Android->env(); //获取当前线程的JNIEnv指针
        /*
         * JNI中FindClass和GetObjectClass都是返回jclass。FindClass是通过传java中完整的类名来查找java的class，
         * 而GetObjectClass是通过传入jni中的一个java的引用来获取该引用的类型。
         * 前者要求你必须知道完整的类名，后者要求在Jni有一个类的引用。
         */
        jclass javaDebugClass = env_new->GetObjectClass(debugObject_Android->javaDedebugObj);
        jmethodID mid = env_new->GetStaticMethodID(javaDebugClass, "throwJNIException", "(Ljava/lang/String;Ljava/lang/String;)V");
        jstring errorstr = (*env_new).NewStringUTF(errorStr.c_str());
        jstring stackstr = (*env_new).NewStringUTF(((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->stacktraceMessage.c_str());
        env_new->CallStaticVoidMethod(javaDebugClass, mid, errorstr, stackstr);
    };

    mtglDebug_register();
}

JNIEXPORT jboolean JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_IsEnable(JNIEnv *env, jobject obj) {
    return MTGLDebugCore::isEnable();
}


JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNIDrawCallsPerSeconds(JNIEnv *env, jobject obj) {

    jclass clazz = NULL;
    jmethodID static_method = NULL;
    static jstring str = NULL;
    static int memeroySize = 0;

    clazz = (*env).FindClass("com/meitu/mtgldebug/MTGLDebugJNI");
    if (clazz == NULL) {
        return;
    }

    static_method = (*env).GetStaticMethodID(clazz, "throwJNIDrawCallsPerSeconds", "(Ljava/lang/String;)V");
    if (static_method == NULL) {
        return;
    }

    MTGLDebugCore::fetchOpenGLESResourceMemorySizeAsynchronously([=](int size) {
        memeroySize = size;
    });

    str = env->NewStringUTF(std::to_string(memeroySize).c_str());
    (*env).CallStaticVoidMethod(clazz, static_method, str);

    (*env).DeleteLocalRef(clazz);
    (*env).DeleteLocalRef(str);
}

JNIEXPORT void JNICALL Java_com_meitu_mtgldebug_MTGLDebugJNI_getJNISortObjectsByMemory(JNIEnv *env, jobject obj) {

    jclass list_class = NULL;
    jmethodID list_costruct = NULL;
    jobject list_obj = NULL;
    jmethodID list_addMethod = NULL;

    /*
     * 这里必须要静态变量，不然下面lamada表达式中没办法赋值的，那个式子是在异步线程中得，空间不一样
     */
    static std::vector<MTGLDebugCoreObject *> sortResult;


    list_class = env->FindClass("java/util/ArrayList"); //获得ArrayList类引用
    if (list_class == NULL) {
        std::cout << "list_class is null \n";
    }

    list_costruct = env->GetMethodID(list_class, "<init>", "()V"); //获得得构造函数Id
    list_obj = env->NewObject(list_class, list_costruct);          //创建一个Arraylist集合对象
    //或得Arraylist类中的 add()方法ID，其方法原型为： boolean add(Object object) ;
    list_addMethod = env->GetMethodID(list_class, "add", "(Ljava/lang/Object;)Z");

    jclass MTGLDebugCoreObject_class = env->FindClass("com/meitu/mtgldebug/debugdata/MTGLDebugCoreObject"); //获得MTGLDebugCoreObject类引用
    //jmethodID MTGLDebugCoreObject_costruct = env->GetMethodID(MTGLDebugCoreObject_class , "<init>", "(IIIIJJ)V");
    jmethodID MTGLDebugCoreObject_costruct = env->GetMethodID(MTGLDebugCoreObject_class, "<init>", "()V");


    MTGLDebugCore::sortedDebugObjectsByMemoryAsynchronously([=](std::vector<MTGLDebugCoreObject *> result) {
        sortResult.swap(result);
    });

    for (int i = 0; i < sortResult.size(); i++) {

        //调用默认构造函数，创建默认对象，之后调用set方法来传递参数
        jobject MTGLDebugCore_obj = env->NewObject(MTGLDebugCoreObject_class, MTGLDebugCoreObject_costruct);

        jmethodID setmClassType = env->GetMethodID(MTGLDebugCoreObject_class, "setmClassType", "(I)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmClassType, sortResult[i]->classType());

        jmethodID setmTargetMethod = env->GetMethodID(MTGLDebugCoreObject_class, "setmTarget", "(I)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmTargetMethod, sortResult[i]->target());

        jmethodID setmObjectMethod = env->GetMethodID(MTGLDebugCoreObject_class, "setmObject", "(I)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmObjectMethod, sortResult[i]->object());

        jmethodID setmMemorySizeMethod = env->GetMethodID(MTGLDebugCoreObject_class, "setmMemorySize", "(I)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmMemorySizeMethod, sortResult[i]->memorySize());

        jmethodID setmContextMethod = env->GetMethodID(MTGLDebugCoreObject_class, "setmContext", "(J)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmContextMethod, sortResult[i]->context());

        jmethodID setmSharegroupMethod = env->GetMethodID(MTGLDebugCoreObject_class, "setmSharegroup", "(J)V");
        env->CallVoidMethod(MTGLDebugCore_obj, setmSharegroupMethod, sortResult[i]->sharegroup());

        //调用ArrayList函数的add方法，来将刚才创建的对象添加入list中
        env->CallBooleanMethod(list_obj, list_addMethod, MTGLDebugCore_obj);
    }

    jclass callback_clazz = NULL;
    jmethodID callbackstatic_method = NULL;

    callback_clazz = (*env).FindClass("com/meitu/mtgldebug/MTGLDebugJNI");
    if (callback_clazz == NULL) {
        return;
    }

    callbackstatic_method = (*env).GetStaticMethodID(callback_clazz, "throwJNISortObjectsByMemory", "(Ljava/util/ArrayList;)V");
    if (callbackstatic_method == NULL) {
        return;
    }

    (*env).CallStaticVoidMethod(callback_clazz, callbackstatic_method, list_obj);


    (*env).DeleteLocalRef(callback_clazz);
    (*env).DeleteLocalRef(list_class);
    (*env).DeleteLocalRef(MTGLDebugCoreObject_class);
}

#endif
