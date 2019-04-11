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


#include <jni.h>
#include "../Core/MTGLDebugCore.hpp"

namespace GLDebug {

struct AndroidContext {
  public:
    EGLContext context;
    EGLContext shareContext;
    MTGLDebugCoreGLAPIVersion glAPI = MTGLDebugCoreGLAPIVersionGLES2;

    AndroidContext() = default;

    AndroidContext(EGLContext ctx, EGLContext shareCtx, MTGLDebugCoreGLAPIVersion API) {
        context = ctx;
        shareContext = shareCtx ? shareCtx : ctx;
        glAPI = API;
    }

    AndroidContext(const AndroidContext &androidContext) {
        context = androidContext.context;
        shareContext = androidContext.shareContext;
        glAPI = androidContext.glAPI;
    }

    AndroidContext &operator=(const AndroidContext &androidContext) {
        context = androidContext.context;
        shareContext = androidContext.shareContext;
        glAPI = androidContext.glAPI;
        return *this;
    }
};

class MTGLDebugCore_Android : public MTGLDebugCore
{

  public:
    /* 当前上下文与shareContext绑定，如果shareContext == NULL,那么上下文的shareContext为自身*/
    void bindContext(EGLContext context, EGLContext shareContext, const EGLint *attrib);

    std::function<void(const std::string &errorMessage)> errorExceptionCallback = nullptr;
    std::function<std::string(void)> currentStacktraceCallback = nullptr;
    std::string stacktraceMessage;

    JNIEnv *env() const;

    MTGLDebugCore_Android(JNIEnv *env);

    jobject javaDedebugObj = nullptr;

    static JNIEnv *getENVForJVM(JavaVM *javaVM);
    JavaVM *mGLDebugQueueJavaVM = nullptr; //这个实例表示的是JavaVM指针

    AndroidContext _currentContext();

  protected:
    std::map<const void *, AndroidContext> mContextMap;


    MTGLDebugCoreGLAPIVersion currentGLAPIVersion(void);

    JNIEnv *mGLDebugQueueEnv = nullptr;
};

} // namespace GLDebug
