//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/12/29
// Created by: Zed
//


#import "MTGLDebugCore_iOS.h"
#import <OpenGLES/EAGL.h>
#include <execinfo.h>
#include <mach/vm_types.h>
#include <sys/time.h>
#include "MTGLDebug_Macro.h"

using namespace GLDebug;

bool MTGLDebugCore_iOS::deleteObject(const GLDebug::MTGLDebugCoreInputObject &inputObject) {

    if (!MTGLDebugCore::isEnable()) {
        return true;
    }

    if (inputObject.object == 0) {
        return true;
    }

    const void *contextSharegroup = currentSharegroup();
    MTGLDebugCoreObjectMap *shareGroupObjects = NULL;
    bool result = MTGLDebugCore::checkObject(inputObject,
        &shareGroupObjects, nil, true);
    runSyncBlockInTheSafeThread([&] {
        if (result) {
            MTGLDebugCoreObject *debugObject = MTGLDebugCore::fetchDebugObjectPublic(contextSharegroup, inputObject.target, inputObject.object);

            if (debugObject == NULL) {
                //如果为空 然后符合一下三种情况不做删除操作
                assert(!isEnable() || isEnableOnlyStatisticalGLObject() || inputObject.object == 0);
                return;
            }

            if (debugObject->classType() == MTGLDebugCoreObjectCVTextureClass) {

                MTGLDebugCVCoreObject *cvObject = (MTGLDebugCVCoreObject *)debugObject;

                if (!cvObject->isFromCamera) {
                    NSLog(@"主动删除了CVOpenGLESTextureRef，请调用CVOpenGLESTextureCacheFlush刷新缓存!!");
                }

                runAsyncBlockInTheSafeThread([=] {
                    shareGroupObjects->erase(cvObject->child()->objectID());
                    shareGroupObjects->erase(cvObject->objectID());
                    delete cvObject;
                    MTGLDebugCore::sharedMTGLDebugCore()->flushMTGLDebugCacheObject();
                });
            } else {
                runAsyncBlockInTheSafeThread([=] {
                    auto key = debugObject->objectID();
                    shareGroupObjects->erase(key);
                    delete debugObject;
                    MTGLDebugCore::sharedMTGLDebugCore()->flushMTGLDebugCacheObject();
                });
            }
        }
    });

    return result;
}


#ifdef __cplusplus
extern "C" {
#endif
const void *currentContext() {
    if (MTGLDebugCore::isEnableGLDebugException()) {
        MTGLDEBUG_assert(!GLDebug::MTGLDebugCore::debugQueue()->isCurrentQueue(), "获取上下文资源不应在Debug线程");
    }
    return (__bridge void *)[EAGLContext currentContext];
}

const void *currentSharegroup() {
    if (MTGLDebugCore::isEnableGLDebugException()) {
        MTGLDEBUG_assert(!GLDebug::MTGLDebugCore::debugQueue()->isCurrentQueue(), "获取上下文资源不应在Debug线程");
    }
    return (__bridge void *)[EAGLContext currentContext].sharegroup;
}

#ifdef __cplusplus
}
#endif


MTGLDebugCoreGLAPIVersion MTGLDebugCore_iOS::currentGLAPIVersion() {
    EAGLContext *context = (__bridge EAGLContext *)currentContext();
    return (MTGLDebugCoreGLAPIVersion)context.API;
}
