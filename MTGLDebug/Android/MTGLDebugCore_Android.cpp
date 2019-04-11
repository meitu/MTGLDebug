//
// Copyright (c) 2019-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/12/29
// Created by: ZhangXiaoJun
//


#import "MTGLDebugCore_Android.h"
#include "../../../CplusplusBase/CPPBase/GL/gl.h"

using namespace GLDebug;



MTGLDebugCore_Android::MTGLDebugCore_Android(JNIEnv *env)
    : MTGLDebugCore() {
    /*
     * JNIEnv类型是一个指向全部JNI方法的指针。该指针只在创建它的线程有效，不能跨线程传递
     * JavaVM是虚拟机在JNI中的表示，一个JVM中只有一个JavaVM对象，这个对象是线程共享的
     *       GetJavaVM(&mGLDebugQueueJavaVM);表示获取JVM指针并赋值给mGLDebugQueueJavaVM
     */
    env->GetJavaVM(&mGLDebugQueueJavaVM);
    mDebugQueue->addSync([=] {
        mGLDebugQueueEnv = MTGLDebugCore_Android::getENVForJVM(mGLDebugQueueJavaVM); //获取当前线程的JNIEnv指针
    });
}

JNIEnv *MTGLDebugCore_Android::getENVForJVM(JavaVM *javaVM) {
    JNIEnv *env_new = NULL;
    int getEnvStat = javaVM->GetEnv((void **)&env_new, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        /*
         * jint AttachCurrentThread(JavaVM* vm , JNIEnv** env , JavaVMAttachArgs* args);
         * vm：虚拟机对象指针。
         * env：用来保存得到的JNIEnv的指针。
         * args：链接参数，参数结构体如下所示。
         * return：链接成功返回0，连接失败返回其他。
         *
         * JNIEnv指针仅在创建它的线程有效。如果我们需要在其他线程访问JVM，那么必须先调用AttachCurrentThread将当前线程
         * 与JVM进行关联，然后才能获得JNIEnv对象。当然，我们在必要时需要调用DetachCurrentThread来解除链接。
         */
        if (javaVM->AttachCurrentThread(&env_new, NULL) != 0) {
            mtcb_printf("Failed to attach");
        }
    } else if (getEnvStat == JNI_OK) {
        //
    } else if (getEnvStat == JNI_EVERSION) {
        mtcb_printf("GetEnv: version not supported");
    }
    return env_new;
}



JNIEnv *MTGLDebugCore_Android::env() const {
    return mGLDebugQueueEnv;
}

void MTGLDebugCore_Android::bindContext(EGLContext context, EGLContext shareContext, const EGLint *attrib) {

    runAsyncBlockInTheSafeThread([=] {
        MTGLDebugCoreGLAPIVersion glAPI;
        if (attrib) {

            // {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE}
            glAPI = (MTGLDebugCoreGLAPIVersion)(attrib[1]);

        } else {
            glAPI = MTGLDebugCoreGLAPIVersionGLES1;
        }

        auto pair = std::make_pair<const void *, AndroidContext>((const void *)context, AndroidContext(context, shareContext, glAPI));
        auto findObj = mContextMap.find((const void *)context);

        if (findObj != mContextMap.end()) {
            mContextMap.erase(findObj);
        }

        mContextMap.insert(pair);
    });
}



MTGLDebugCoreGLAPIVersion MTGLDebugCore_Android::currentGLAPIVersion(void) {
    return _currentContext().glAPI;
}

AndroidContext MTGLDebugCore_Android::_currentContext() {
    AndroidContext androidContext;
    const void *context = currentContext();
    runSyncBlockInTheSafeThread([&] {
        assert(context);

        if (!context) {
            _throw("xxx");
        }

        if (context) {
            androidContext = mContextMap[context];
        }
    });
    return androidContext;
}

#ifdef __cplusplus
extern "C" {
#endif
const void *currentSharegroup(void) {
    return ((MTGLDebugCore_Android *)MTGLDebugCore_Android::sharedMTGLDebugCore())->_currentContext().shareContext;
}

const void *currentContext(void) {
    if (MTGLDebugCore::isEnableGLDebugException()) {
        mtcb_assert(!GLDebug::MTGLDebugCore::debugQueue()->isCurrentQueue(), "获取上下文资源不应在Debug线程");
    }

    return eglGetCurrentContext();
}


#ifdef __cplusplus
}
#endif