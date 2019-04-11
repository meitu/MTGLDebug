//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/12/27
// Created by: Zed
//


#include "MTGLDebugCore.hpp"
#include "MTGLDebugGLHookFunction.h"
#include "MTGLDebug_Macro.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE
#include "MTGLDebugCore_iOS.h"
#endif

static GLDebug::MTGLDebugCore *s_sharedDebugObject = nullptr; //C++11新特性，申明一个nullptr型空指针

const std::type_info *kGLEnumType = &typeid(GLenum);
const std::type_info *kGLUIntPtrType = &typeid(GLuint *);
const std::type_info *kGLUIntType = &typeid(GLuint);
const std::type_info *kGLSizeiType = &typeid(GLsizei);
const std::type_info *kGLIntType = &typeid(GLint);
const std::type_info *kGLVoidPtrType = &typeid(GLvoid *);
const std::type_info *kConstGLUIntPtr = &typeid(const GLuint *);
const std::type_info *kGLSizeiPtrType = &typeid(GLsizeiptr);
const std::type_info *kConstGLVoidPtrType = &typeid(const GLvoid *);
const std::type_info *kGLIntPtrType = &typeid(GLintptr);
const std::type_info *kGLBooleanType = &typeid(GLboolean);
const std::type_info *kGLFloatType = &typeid(GLfloat);
const std::type_info *kGLFloatPtrType = &typeid(GLfloat *);
const std::type_info *kGLBitfieldType = &typeid(GLbitfield);


#ifdef MTGLDEBUG_PLATFORM_ANDROID
const std::type_info *kEGLDisplayType = &typeid(EGLDisplay);
const std::type_info *kEGLConfigType = &typeid(EGLConfig);
const std::type_info *kEGLContextType = &typeid(EGLContext);
const std::type_info *kConstEGLintPtrType = &typeid(const EGLint *);
#include "../Android/MTGLDebugCore_Android.h"
#endif

using namespace GLDebug;

#pragma mark-----DictionaryValueForKey-----

void MTGLDebugCoreValueForKey(MTGLDebugSharegroupsMap *map,
    std::string key,
    MTGLDebugCoreObjectMap **value) {
    auto it = map->find(key);
    if (it != map->end()) {
        *value = it->second; //map容器中得frist和second变量，表示的是map的key和value
    }
}

void MTGLDebugCoreValueForKey(MTGLDebugCoreObjectMap *map,
    std::string key,
    MTGLDebugCoreObject **value) {
    auto it = map->find(key);
    if (it != map->end()) {
        *value = it->second;
    }
}


#pragma mark-----MTGLDebugCore-----

MTGLDebugCore::~MTGLDebugCore() {
    cacheObject = NULL;
    delete mShareGroup;
    delete mHistoryShareGroups;
    delete mDrawCallsPerSeconds;
    delete mDebugQueue;
}

MTGLDebugCore::MTGLDebugCore() {
    mProjectName = std::string();

    mShareGroup = new MTGLDebugSharegroupsMap();
    std::string sharegroupID = MTGLDebugCore::sharegroupIDWithAddress(NULL);
    std::map<std::string, MTGLDebugCoreObject *> *nullMap = new std::map<std::string, MTGLDebugCoreObject *>();
    mShareGroup->insert(std::pair<std::string, std::map<std::string, MTGLDebugCoreObject *> *>(sharegroupID, nullMap));
    mHistoryShareGroups = new MTGLDebugSharegroupsMap();
    mHistoryShareGroups->insert(std::pair<std::string, std::map<std::string, MTGLDebugCoreObject *> *>(sharegroupID, nullMap));

    mDrawCallsPerSeconds = new std::vector<double>();
    mDebugQueue = new GLDebug::Queue();
    mDebugQueue->setName("com.meitu.MTGLDebug");
    mDebugOptions = defaultDebugOptions();
}

MTGLDebugCore *MTGLDebugCore::sharedMTGLDebugCore() {
    static std::once_flag flag;
    std::call_once(flag, []() {
#ifndef MTGLDEBUG_PLATFORM_ANDROID
        s_sharedDebugObject = new MTGLDebugCore_iOS();
#endif
    });
    return s_sharedDebugObject;
}

#ifdef MTGLDEBUG_PLATFORM_ANDROID
void MTGLDebugCore::initSharedGLDebugObject(JNIEnv *env) {
    static std::once_flag flag; //once_flag关键字表示给flag加锁
    /*
	 * 如果多个线程需要同时调用某个函数，std::call_once可以保证多个线程对该函数只调用一次
	 * flag:相当于Java中的锁标志，即给flag加锁
	 * [=] :C++11提供了对匿名函数的支持,称为Lambda函数。
	 *      []为Lambda引入符，作用就是表明其后的 lambda 表达式以何种方式使用这些变量（正式的术语是“捕获”）。
	 *      [=]表示lambda 表达式以传值的形式捕获（使用）同范围内的变量。
	 * 详见https://www.devbean.net/2012/05/cpp11-lambda/
	 */
    std::call_once(flag, [=]() {
        s_sharedDebugObject = new MTGLDebugCore_Android(env); //这其中顺便获取了当前线程的JNIEnv指针
    });
}

#endif


GLDebug::Queue *MTGLDebugCore::debugQueue(void) {
    return MTGLDebugCore::sharedMTGLDebugCore()->mDebugQueue;
}

MTGLDebugImmutableSharegroupsMap MTGLDebugCore::immutableSharegroups(void) {
    MTGLDebugImmutableSharegroupsMap copySharegroups = *(MTGLDebugImmutableSharegroupsMap *)sharedMTGLDebugCore()->mShareGroup;
    return copySharegroups;
}

MTGLDebugSharegroupsMap *MTGLDebugCore::sharegroups(void) {
    return MTGLDebugCore::sharedMTGLDebugCore()->mShareGroup;
}

MTGLDebugImmutableSharegroupsMap MTGLDebugCore::immutableHistorySharegroups(void) {
    MTGLDebugImmutableSharegroupsMap copySharegroups = *(MTGLDebugImmutableSharegroupsMap *)sharedMTGLDebugCore()->mHistoryShareGroups;
    return copySharegroups;
}

MTGLDebugSharegroupsMap *MTGLDebugCore::historySharegroups(void) {
    return MTGLDebugCore::sharedMTGLDebugCore()->mHistoryShareGroups;
}

void MTGLDebugCore::runBlockInTheSafeThread(std::function<void()> block, bool isAsync) {
    if (this->mDebugQueue->isCurrentQueue()) {
        block();
    } else {
        this->mDebugQueue->addOperation(block, isAsync);
    }
}

void MTGLDebugCore::runAsyncBlockInTheSafeThread(std::function<void()> block) {
    MTGLDebugCore::sharedMTGLDebugCore()->runBlockInTheSafeThread(block, true);
}


void MTGLDebugCore::runSyncBlockInTheSafeThread(std::function<void()> block) {
    MTGLDebugCore::sharedMTGLDebugCore()->runBlockInTheSafeThread(block, false);
}

std::string MTGLDebugCore::sharegroupIDWithAddress(const void *contextSharegroup) {
    static char sharegroupID[50];
    sprintf(sharegroupID, "sharegroup:%p", contextSharegroup); //格式化字符串 "sharegroup:参数" 并输出到 sharegroupID 代表的字符串中
    return sharegroupID;
}


void MTGLDebugCore::SetGLDebugException(bool enable) {
    MTGLDebugCoreOptions options;
    if (enable) {
        options = (MTGLDebugCoreOptions)(sharedMTGLDebugCore()->mDebugOptions & (~MTGLDebugCoreOptionsDisableException));
    } else {
        options = (MTGLDebugCoreOptions)(sharedMTGLDebugCore()->mDebugOptions | (MTGLDebugCoreOptionsDisableException));
    }
    MTGLDebugCore::SetDebugOptions(options);
}

void MTGLDebugCore::SetGLDebugOnlyStatisticalGLObject(bool enable) {
    MTGLDebugCoreOptions options;
    if (enable) {
        //关闭ApiGetError和上下文检测
        options = (MTGLDebugCoreOptions)(sharedMTGLDebugCore()->mDebugOptions & (~MTGLDebugCoreOptionsCheckAPIUsageStates));
        options = (MTGLDebugCoreOptions)(options & (~MTGLDebugCoreOptionsCheckContextStates));
        options = (MTGLDebugCoreOptions)(options | (MTGLDebugCoreOptionsOnlyStatisticalGLObject));
    } else {
        options = (MTGLDebugCoreOptions)(sharedMTGLDebugCore()->mDebugOptions | MTGLDebugCoreOptionsCheckAPIUsageStates);
        options = (MTGLDebugCoreOptions)(options | MTGLDebugCoreOptionsCheckContextStates);
        options = (MTGLDebugCoreOptions)(options & (~MTGLDebugCoreOptionsOnlyStatisticalGLObject));
    }
    MTGLDebugCore::SetDebugOptions(options);
}

MTGLDebugCoreObjectMap *MTGLDebugCore::getLazyShareGroupObjects(MTGLDebugSharegroupsMap *sharegroups, std::string sharegroupID) {
    MTGLDebugCoreObjectMap *shareGroupObjects = NULL;
    MTGLDebugCoreValueForKey(sharegroups, sharegroupID, &shareGroupObjects);
    if (!shareGroupObjects) {
        shareGroupObjects = new MTGLDebugCoreObjectMap();
        (*sharegroups)[sharegroupID] = shareGroupObjects;
    }
    return shareGroupObjects;
}

MTGLDebugCoreObjectMap *MTGLDebugCore::getLazyHistorySharegroupObjects(MTGLDebugSharegroupsMap *historySharegroups, std::string sharegroupID) {
    MTGLDebugCoreObjectMap *historySharegroupObjects = NULL;
    MTGLDebugCoreValueForKey(historySharegroups, sharegroupID, &historySharegroupObjects);
    if (!historySharegroupObjects) {
        historySharegroupObjects = new MTGLDebugCoreObjectMap();
        (*historySharegroups)[sharegroupID] = historySharegroupObjects;
    }
    return historySharegroupObjects;
}


#pragma mark-----MTGLDebugCoreOperation-----
/**
 OnlyStatisticalGLObject 不记录历史对象
*/
MTGLDebugCoreObject *MTGLDebugCore::addObject(MTGLDebugCoreInputObject inputObject, size_t memorySize) {

    const void *context = currentContext();
    const void *sharegroup = currentSharegroup();

    if (inputObject.object == 0) {
        return NULL;
    }
#ifdef MTGLDEBUG_PLATFORM_ANDROID
    ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->stacktraceMessage = ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->currentStacktraceCallback();

    LOGE("addObjectsFunc.cpp =====>:%d  obj === %d  context ===%p  sharegroup ===%p", inputObject.target, inputObject.object, context, sharegroup);
#endif


    MTGLDebugCoreObject *debugObject = NULL;
    MTGLDebugCore::runSyncBlockInTheSafeThread([&](void) mutable -> void {
        std::string sharegroupID = MTGLDebugCore::sharedMTGLDebugCore()->sharegroupIDWithAddress(sharegroup);
        auto sharegroups = MTGLDebugCore::sharedMTGLDebugCore()->sharegroups(); //MTGLDebugSharegroupsMap
        auto historySharegroups = MTGLDebugCore::sharedMTGLDebugCore()->historySharegroups();
        //输入一个 sharegroups
        MTGLDebugCoreObjectMap *shareGroupObjects = getLazyShareGroupObjects(sharegroups, sharegroupID);
        MTGLDebugCoreObjectMap *historySharegroupObjects = getLazyHistorySharegroupObjects(historySharegroups, sharegroupID);

        switch (inputObject.initClass) {
            case MTGLDebugCoreObjectBaseClass:
                debugObject = new MTGLDebugCoreObject(inputObject.target, inputObject.object, context, sharegroup, memorySize);
                break;
            case MTGLDebugCoreObjectTextureClass:
                debugObject = new MTGLDebugTextureCoreObject(inputObject.target, inputObject.object, context, sharegroup, memorySize);
                break;
            case MTGLDebugProgramCoreObjectClass:
                debugObject = new MTGLDebugProgramCoreObject(inputObject.target, inputObject.object, context, sharegroup, memorySize);
                break;
            case MTGLDebugFramebuffeCoreObjectClass:
                debugObject = new MTGLDebugFramebuffeCoreObject(inputObject.target,
                    inputObject.object,
                    context,
                    sharegroup,
                    memorySize);
                break;

            default:
                break;
        }

        MTGLDebugCoreObject *objectCache = NULL;
        std::string objectID = debugObject->objectID();

        MTGLDebugCoreValueForKey(shareGroupObjects, objectID, &objectCache);

        if (objectCache == NULL) {
            shareGroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(objectID, debugObject));

        } else {
            delete debugObject;
            debugObject = objectCache;
#ifdef MTGLDEBUG_PLATFORM_ANDROID
            debugObject->retain();
#else
            if (!MTGLDebugCore::isEnableGLDebugException()) {
                MTGLDEBUG_assert(false, "逻辑错误！！！！！此处不应有缓存");
            }
#endif
        }

        historySharegroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(objectID, debugObject));
    });
    return debugObject;
}

#ifdef MTGLDEBUG_PLATFORM_APPLE
void MTGLDebugCore::addCVObject(const MTGLDebugCoreInputObject &parentInputObject, size_t parentMemorySize,
    const MTGLDebugCoreInputObject &childInputObject, size_t childMemorySize) {
    const void *context = currentContext();
    const void *sharegroup = currentSharegroup();

    if (parentInputObject.object == 0 || childInputObject.object == 0) {
        MTGLDEBUG_printf("CVOpenGLESCache缓存为空");
        return;
    }

    MTGLDebugCore::runSyncBlockInTheSafeThread([&](void) -> void {
        std::string sharegroupID = MTGLDebugCore::sharedMTGLDebugCore()->sharegroupIDWithAddress(sharegroup);
        auto sharegroups = MTGLDebugCore::sharedMTGLDebugCore()->sharegroups();
        MTGLDebugCoreObjectMap *shareGroupObjects = getLazyShareGroupObjects(sharegroups, sharegroupID);

        auto historySharegroups = MTGLDebugCore::sharedMTGLDebugCore()->historySharegroups();
        MTGLDebugCoreObjectMap *historySharegroupObjects = getLazyHistorySharegroupObjects(historySharegroups, sharegroupID);


        MTGLDebugTextureCoreObject *childDebugObject = new MTGLDebugTextureCoreObject(childInputObject.target,
            childInputObject.object,
            context, sharegroup,
            childMemorySize);


        MTGLDebugCVCoreObject *parentDebugObject = new MTGLDebugCVCoreObject(parentInputObject.target,
            parentInputObject.object,
            context,
            sharegroup,
            parentMemorySize);

        parentDebugObject->setChild(childDebugObject);

        MTGLDebugCoreObject *parentDebugObjectCache = NULL;
        MTGLDebugCoreObject *childDebugObjectCache = NULL;

        MTGLDebugCoreValueForKey(shareGroupObjects, parentDebugObject->objectID(), &parentDebugObjectCache);
        MTGLDebugCoreValueForKey(shareGroupObjects, childDebugObject->objectID(), &childDebugObjectCache);

        if ((parentDebugObjectCache == NULL) && (childDebugObjectCache == NULL)) {
            shareGroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(parentDebugObject->objectID(), parentDebugObject));
            shareGroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(childDebugObject->objectID(), childDebugObject));
        } else {
            if (!MTGLDebugCore::isEnableGLDebugException()) {
                MTGLDEBUG_assert(false, "逻辑错误！！！！！此处不应有缓存");
            }
        }

        historySharegroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(parentDebugObject->objectID(), parentDebugObject));
        historySharegroupObjects->insert(std::pair<std::string, MTGLDebugCoreObject *>(childDebugObject->objectID(), childDebugObject));
    });
}
#endif

bool MTGLDebugCore::checkObject(const MTGLDebugCoreInputObject &inputObject,
    MTGLDebugCoreObjectMap **outputShareGroupObjects,
    unsigned int *outputErrorType,
    bool isDelete,
    const void *context,
    const void *sharegroup) {
    if (inputObject.object == 0) {
        return true;
    }

    bool result = false;
    MTGLDebugCore::runSyncBlockInTheSafeThread([=, &result]() mutable {
        std::string sharegroupID = MTGLDebugCore::sharedMTGLDebugCore()->sharegroupIDWithAddress(sharegroup);
        auto sharegroups = MTGLDebugCore::sharedMTGLDebugCore()->sharegroups();
        MTGLDebugCoreObjectMap *shareGroupObjects = getLazyShareGroupObjects(sharegroups, sharegroupID);

        if (MTGLDebugCore::isEnableOnlyStatisticalGLObject()) {
            if (outputShareGroupObjects && shareGroupObjects) {
                *outputShareGroupObjects = shareGroupObjects;
                result = true;
            } else {
                result = false;
            }
            return;
        }

        //当前要查找的对象
        MTGLDebugCoreObject *debugObject = NULL;
        switch (inputObject.initClass) {
            case MTGLDebugCoreObjectBaseClass:
                debugObject = new MTGLDebugCoreObject(inputObject.target, inputObject.object, context, sharegroup, 0);
                break;

            case MTGLDebugCoreObjectTextureClass:
                debugObject = new MTGLDebugTextureCoreObject(inputObject.target, inputObject.object, context, sharegroup, 0);
                break;

#ifdef MTGLDEBUG_PLATFORM_APPLE
            case MTGLDebugCoreObjectCVTextureClass: {
                debugObject = new MTGLDebugCVCoreObject(inputObject.target,
                    inputObject.object,
                    context,
                    sharegroup,
                    0);
            } break;
#endif
            case MTGLDebugProgramCoreObjectClass:
                debugObject = new MTGLDebugProgramCoreObject(inputObject.target, inputObject.object, context, sharegroup, 0);
                break;
            case MTGLDebugFramebuffeCoreObjectClass: {
                debugObject = new MTGLDebugFramebuffeCoreObject(inputObject.target,
                    inputObject.object,
                    context,
                    sharegroup,
                    0);
            } break;
            default:
                break;
        }

        //当前上下文查找
        MTGLDebugCoreObject *originalDebugObject = NULL;
        MTGLDebugCoreValueForKey(shareGroupObjects, debugObject->objectID(), &originalDebugObject);
        result = originalDebugObject != NULL; //非0转换为true，0转换为false


        //历史记录的GL对象(可能被删除了)
        MTGLDebugCoreObject *recordDebugObject = NULL;
        auto historySharegroups = MTGLDebugCore::sharedMTGLDebugCore()->historySharegroups();
        MTGLDebugCoreObjectMap *historyShareGroupObjects = getLazyHistorySharegroupObjects(historySharegroups, sharegroupID);
        MTGLDebugCoreValueForKey(historyShareGroupObjects, debugObject->objectID(), &recordDebugObject);

        if (outputShareGroupObjects && shareGroupObjects) {
            *outputShareGroupObjects = shareGroupObjects;
        }

        MTGLDebugObjectStatus objectStatus = MTGLDebugObjectStatusNotExistGLObject;

        if (!result) { //reslut为false，说明originalDebugObject为0,也就是在shareGroupObjects中查不到要删的对象ID，此时要出错了
            //检验下这个temp是否是另一个地址
            MTGLDebugImmutableSharegroupsMap tempShareGroups = MTGLDebugCore::sharedMTGLDebugCore()->immutableSharegroups();

            tempShareGroups.erase(sharegroupID);

            for (auto tempShareGroupsItems : tempShareGroups) {
                MTGLDebugCoreObjectImmutableMap *tempShareGroupObjects = tempShareGroupsItems.second;
                const MTGLDebugCoreObject *tempDebugObject = NULL;

                auto it = tempShareGroupObjects->find(debugObject->objectID());
                if (it != tempShareGroupObjects->end()) {
                    tempDebugObject = it->second;
                }

                if (tempDebugObject) {
                    objectStatus = MTGLDebugObjectStatusUseWronglyGLObject;
                    break;
                }
            }

            if (objectStatus == MTGLDebugObjectStatusUseWronglyGLObject) {
                if (recordDebugObject) {
                    objectStatus = MTGLDebugObjectStatusUseWronglyDeleteGLObject;
                } else {
                    objectStatus = MTGLDebugObjectStatusUseWronglyOperateGLObject;
                }
            } else {
                if (recordDebugObject) {
                    objectStatus = MTGLDebugObjectStatusUseForwardlyDeletedGLObject;
                } else {
                    objectStatus = MTGLDebugObjectStatusNotExistGLObject;
                }
            }

            std::string errorString;
            switch (objectStatus) {
                case MTGLDebugObjectStatusNotExistGLObject:
                    errorString = "当前上下文不包含这个对象!!!这个GL对象不存在,从未被创建过(在任何上下文都没有创建成功)";
                    break;
                case MTGLDebugObjectStatusUseWronglyDeleteGLObject:
                    errorString = "当前上下文不包含这个对象!!!对不同上下文中相同id(因绑定错上下文导致误删除)GL对象进行删除操作!!!你可能要删除的对象是" + debugObject->debugDescription();
                    break;
                case MTGLDebugObjectStatusUseWronglyOperateGLObject:
                    errorString = "当前上下文不包含这个对象!!!对不同上下文中相同id的GL对象进行操作!!!你可能要操作的对象是" + debugObject->debugDescription();
                    break;
                case MTGLDebugObjectStatusUseForwardlyDeletedGLObject:
                    errorString = "当前上下文不包含这个对象!!!!!!在当前上下文创建过但是被销毁,使用已被删除的对象" + debugObject->debugDescription();
                    break;
                default:
                    break;
            }
            GLDebug::_throw(errorString, MTGLDebugCoreErrorTypeError);

            if (outputErrorType) {
                *outputErrorType = objectStatus;
            }
        } else {
            if (!isDelete) {
                if (MTGLDebugFramebuffeCoreObjectClass == inputObject.initClass) {
                    MTGLDebugFramebuffeCoreObject *fbo = static_cast<MTGLDebugFramebuffeCoreObject *>(originalDebugObject);
                    if (fbo->isBindingTexture()) {
                        MTGLDebugCoreInputObject inputObject;
                        inputObject.object = fbo->getTexture();
                        inputObject.target = 0x4;                                //MT_GLDEBUG_TEXTURE
                        inputObject.initClass = MTGLDebugCoreObjectTextureClass; //MTGLDebugCoreObjectTexture

                        result = checkObject(inputObject,
                            NULL,
                            NULL,
                            false,
                            context,
                            sharegroup);
                        if (!result) {
                            fbo->setIsBindingTexture(false);
                        }
                    }
                }
            }
        }

        delete debugObject;
    });

    return result;
}


void MTGLDebugCore::deleteObjectsFunc(int count, const unsigned int *params, unsigned int target, int initClass) {
    if (!count) {
        return;
    }
    for (int i = 0; i < count; i++) {
        unsigned int object = params[i];
        MTGLDebugCoreInputObject inputObj;
        inputObj.object = object;
        inputObj.target = target;

        inputObj.initClass = initClass;
        MTGLDebugCore::sharedMTGLDebugCore()->deleteObject(inputObj);
    }
}

bool MTGLDebugCore::deleteObject(const MTGLDebugCoreInputObject &inputObject) {
    const void *sharegroup = currentSharegroup();

    if (inputObject.object == 0) {
        return false;
    }

#ifdef MTGLDEBUG_PLATFORM_ANDROID

    LOGE("deleteObjectsFunc.cpp =====>:%d  obj === %d  context ===%p  sharegroup ===%p", inputObject.target, inputObject.object, currentContext(), sharegroup);

    ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->stacktraceMessage = ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->currentStacktraceCallback();
#endif


    MTGLDebugCoreObjectMap *shareGroupObjects = NULL;
    bool result = MTGLDebugCore::checkObject(inputObject, &shareGroupObjects, NULL, true);
    if (result) {
        MTGLDebugCore::runSyncBlockInTheSafeThread([&] {
            MTGLDebugCoreObject *debugObject = fetchDebugObject(sharegroup, inputObject.target, inputObject.object);

#ifdef MTGLDEBUG_PLATFORM_ANDROID
            debugObject->release();

            if (!debugObject->referenceCount()) {
                shareGroupObjects->erase(debugObject->objectID());
                delete debugObject;
            }
#else
            shareGroupObjects->erase(debugObject->objectID());
            delete debugObject;
#endif
        });
    }

    return result;
}



#pragma mark-----MTGLDebugCoreCalutate-----

size_t MTGLDebugCore::fetchEAGLSharegroupMemorySize(void *sharegroup) {
    size_t memorySize = 0;
    MTGLDebugCore::runSyncBlockInTheSafeThread([&]() mutable {
        std::string sharegroupID = GLDebug::MTGLDebugCore::sharegroupIDWithAddress(sharegroup);
        MTGLDebugCoreObjectMap *shareGroupObjects = NULL;
        MTGLDebugSharegroupsMap *sharegroupMap = MTGLDebugCore::GetSharegroups();
        MTGLDebugCoreValueForKey(sharegroupMap, sharegroupID, &shareGroupObjects);
        for (auto obj : *shareGroupObjects) {
            memorySize += obj.second->memorySize();
        }
    });
    return memorySize;
}


size_t MTGLDebugCore::fetchOpenGLESResourceMemorySize(void) {
    size_t memorySize = 0;
    MTGLDebugCore::runSyncBlockInTheSafeThread([&]() mutable {
        MTGLDebugSharegroupsMap *shareGroups = MTGLDebugCore::sharedMTGLDebugCore()->GetSharegroups();
        for (auto shareGroupsItems : *shareGroups) {
            MTGLDebugCoreObjectMap *obj = shareGroupsItems.second;
            for (auto objItems : *obj) {
                MTGLDebugCoreObject *debugObj = objItems.second;
                memorySize += debugObj->memorySize();
            }
        }
    });

    return memorySize;
}


void MTGLDebugCore::fetchOpenGLESResourceMemorySizeAsynchronously(std::function<void(size_t size)> completionHandler) {
    MTGLDebugCore::runAsyncBlockInTheSafeThread([=] {
        if (completionHandler) {
            completionHandler(MTGLDebugCore::fetchOpenGLESResourceMemorySize());
        }
    });
}

bool MTGLDebugCore_Compare(MTGLDebugCoreObject *&a, MTGLDebugCoreObject *&b) {
    return *b > *a;
}

std::vector<MTGLDebugCoreObject *> MTGLDebugCore::sortedDebugObjectsByMemory(void) {
    std::vector<MTGLDebugCoreObject *> sortMemoryUsageObject;
    MTGLDebugCore::runSyncBlockInTheSafeThread([&] {
        MTGLDebugImmutableSharegroupsMap shareGroups = MTGLDebugCore::sharedMTGLDebugCore()->immutableSharegroups();
        for (auto shareGroupsItems : shareGroups) {
            MTGLDebugCoreObjectImmutableMap objects = *shareGroupsItems.second;
            std::vector<MTGLDebugCoreObject *> allValues;
            for (auto it = objects.begin(); it != objects.end(); ++it) {

                switch (it->second->classType()) {
                    case MTGLDebugCoreObjectBaseClass:
                        allValues.push_back(new MTGLDebugCoreObject(*it->second));
                        break;
                    case MTGLDebugCoreObjectTextureClass:
                        allValues.push_back(new MTGLDebugTextureCoreObject(*(MTGLDebugTextureCoreObject *)it->second));
                        break;
#ifdef MTGLDEBUG_PLATFORM_APPLE
                    case MTGLDebugCoreObjectCVTextureClass:
                        allValues.push_back(new MTGLDebugCVCoreObject(*(MTGLDebugCVCoreObject *)it->second));
                        break;
#endif
                    case MTGLDebugProgramCoreObjectClass:
                        allValues.push_back(new MTGLDebugProgramCoreObject(*(MTGLDebugProgramCoreObject *)it->second));
                        break;
                    case MTGLDebugFramebuffeCoreObjectClass:
                        allValues.push_back(new MTGLDebugFramebuffeCoreObject(*(MTGLDebugFramebuffeCoreObject *)it->second));
                        break;
                    default:
                        break;
                }
            }
            std::sort(allValues.begin(), allValues.end(), MTGLDebugCore_Compare);
            for (int i = 0; i != allValues.size(); ++i) {
                sortMemoryUsageObject.push_back(allValues.at(i));
            }
        }
    });
    return sortMemoryUsageObject;
}

void MTGLDebugCore::sortedDebugObjectsByMemoryAsynchronously(std::function<void(std::vector<MTGLDebugCoreObject *> result)> completionHandler) {
    MTGLDebugCore::runAsyncBlockInTheSafeThread([=] {
        if (completionHandler) {
            completionHandler(MTGLDebugCore::sortedDebugObjectsByMemory());
        }
    });
}

unsigned int MTGLDebugCore::drawCallsPerSeconds(void) {
    unsigned int count = 0;
    MTGLDebugCore::runSyncBlockInTheSafeThread([&]() mutable {
        std::vector<double> *sharemDrawCallsPerSeconds = MTGLDebugCore::sharedMTGLDebugCore()->mDrawCallsPerSeconds;
        std::vector<double> drawCallsPerSeconds = *sharemDrawCallsPerSeconds;
        double currentTime = std::chrono::system_clock::now().time_since_epoch().count();
        for (auto drawCallsPerSecondsItems : drawCallsPerSeconds) {
            if (currentTime - drawCallsPerSecondsItems > 1) {
                sharemDrawCallsPerSeconds->erase(sharemDrawCallsPerSeconds->begin());
            }
            count = (unsigned int)sharemDrawCallsPerSeconds->size();
        }
    });
    return count;
}

void MTGLDebugCore::didDraw() {
    MTGLDebugCore::runAsyncBlockInTheSafeThread([] {
        std::vector<double> *sharemDrawCallsPerSeconds = MTGLDebugCore::sharedMTGLDebugCore()->mDrawCallsPerSeconds;
        std::vector<double> drawCallsPerSeconds = *sharemDrawCallsPerSeconds;
        double currentTime = std::chrono::system_clock::now().time_since_epoch().count();
        for (auto drawCallsPerSecondsItems : drawCallsPerSeconds) {
            if (currentTime - drawCallsPerSecondsItems > 1) {
                sharemDrawCallsPerSeconds->erase(sharemDrawCallsPerSeconds->begin());
            }
        }
        if (!MTGLDebugCore::isEnable()) {
            return;
        }
        sharemDrawCallsPerSeconds->push_back(std::chrono::system_clock::now().time_since_epoch().count());
    });
}

void MTGLDebugCore::flushMTGLDebugCacheObject() {
    MTGLDebugCore::sharedMTGLDebugCore()->cacheObject = NULL;
}

MTGLDebugCoreObject *MTGLDebugCore::fetchDebugObject(const void *contextShapegoup, GLenum taget, unsigned int object) {

    MTGLDebugCoreObject *obj = NULL;
    GLDebug::MTGLDebugCore::runSyncBlockInTheSafeThread([&] {
        if (MTGLDebugCore::sharedMTGLDebugCore()->cacheObject != NULL) {
            if (*MTGLDebugCore::sharedMTGLDebugCore()->cacheObject->pSharegroup() == contextShapegoup && MTGLDebugCore::sharedMTGLDebugCore()->cacheObject->target() == taget && MTGLDebugCore::sharedMTGLDebugCore()->cacheObject->object() == object) {
                obj = MTGLDebugCore::sharedMTGLDebugCore()->cacheObject;
                return;
            }
        }

        std::string objectId = std::to_string(taget) + "-" + std::to_string(object);
        MTGLDebugSharegroupsMap *sharegroupMap = MTGLDebugCore::sharedMTGLDebugCore()->sharegroups();
        MTGLDebugCoreObjectMap *objMap = NULL;
        std::string sharegroupID = MTGLDebugCore::sharegroupIDWithAddress(contextShapegoup);
        MTGLDebugCoreValueForKey(sharegroupMap, sharegroupID, &objMap);
        MTGLDebugCoreValueForKey(objMap, objectId, &obj);

        MTGLDebugCore::sharedMTGLDebugCore()->cacheObject = obj;
    });

    return obj;
}



void MTGLDebugCore::willCallAPI(const char *methodName, ...) {
    printf("will call api will call api will call api will call api will call api will call api ");
}

void MTGLDebugCore::didCallAPI(const char *methodName, const void *result, ...) {
    printf("did call api did call api did call api did call api did call api did call api ");
}


#pragma mark-----MTGLDebugException-----

std::map<GLenum, std::string> convenienceCreateList(std::initializer_list<GLenum> key,
    std::initializer_list<std::string> value) {
    std::map<GLenum, std::string> result;

    for (long i = 0; i < key.size(); i++) {
        auto kp = key.begin();
        auto sp = value.begin();
        kp += i;
        sp += i;
        std::string s = *sp;
        GLenum k = *kp;
        result.insert(std::make_pair(k, s));
    }
    return result;
}

std::map<MTGLDebugCoreGLAPIVersion, std::map<std::string, MTGLDebugCoreAPIInfo>> &MTGLDebugCore::GetErrorInfo() {

    static std::once_flag flag;
    static std::map<MTGLDebugCoreGLAPIVersion, std::map<std::string, MTGLDebugCoreAPIInfo>> errorInfo;
    std::call_once(flag, []() {
        std::map<std::string, MTGLDebugCoreAPIInfo> tmpErrorInfoES2;

        //        glGenTextures
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.

        tmpErrorInfoES2["glGenTextures"] = MTGLDebugCoreAPIInfo("glGenTextures",
            {"n", "textures"},
            {kGLSizeiType, kGLUIntPtrType},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glBindTexture
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not one of the allowable values.
        //        GL_INVALID_OPERATION is generated if texture was previously created with a target that doesn't match that of target.



        tmpErrorInfoES2["glBindTexture"] = MTGLDebugCoreAPIInfo("glBindTexture",
            {"target", "texture"},
            {kGLEnumType, kGLUIntType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_OPERATION}, {"第一个参数非允许值!\n", "texture与Target不匹配!\n"}));


        //    {TO_MAP(GL_INVALID_ENUM,"第一个参数非允许值!\n"),
        //        TO_MAP(GL_INVALID_OPERATION ,"texture与Target不匹配!\n")};
        //        glTexImage2D
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
        //        GL_INVALID_VALUE is generated if target is one of the six cube map 2D image targets and the width and height parameters are not equal.
        //        GL_INVALID_ENUM is generated if type is not a type constant.
        //        GL_INVALID_VALUE is generated if level is less than 0.
        //        GL_INVALID_VALUE may be generated if level is greater than log2(max)log2⁡max, where max is the returned value of GL_MAX_TEXTURE_SIZE.
        //        GL_INVALID_VALUE is generated if internalFormat is not one of the accepted resolution and format symbolic constants.
        //        GL_INVALID_VALUE is generated if width or height is less than 0 or greater than GL_MAX_TEXTURE_SIZE.
        //        GL_INVALID_VALUE is generated if border is not 0.
        //        GL_INVALID_OPERATION is generated if format does not match internalformat.
        //        GL_INVALID_OPERATION is generated if type is GL_UNSIGNED_SHORT_5_6_5 and format is not GL_RGB.
        //        GL_INVALID_OPERATION is generated if type is GL_UNSIGNED_SHORT_4_4_4_4 or GL_UNSIGNED_SHORT_5_5_5_1 and format is not GL_RGBA.

        std::string glTexImage2DInvalidEnum = "这个错误有以下两种情况:\n";
        glTexImage2DInvalidEnum += "1、target非以下几种GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.\n";
        glTexImage2DInvalidEnum += "2、type参数非支持的type常量\n";


        std::string glTexImage2DInvalidValue = "这个错误有以下六种情况:\n";
        glTexImage2DInvalidValue += "1、target是一个包含六个面的cube map，那么它的宽高必须一致\n";
        glTexImage2DInvalidValue += "2、level参数小于0\n";
        glTexImage2DInvalidValue += "3、level参数大于log2(max)，max等价于GL_MAX_TEXTURE_SIZE\n";
        glTexImage2DInvalidValue += "4、internalFormat参数大于log2(max)，max等价于GL_MAX_TEXTURE_SIZE\n";
        glTexImage2DInvalidValue += "5、width or height 小于0或者大于GL_MAX_TEXTURE_SIZE\n";
        glTexImage2DInvalidValue += "6、border参数非0\n";

        std::string glTexImage2DInvalidOperation = "这个错误有以下三种情况:\n";
        glTexImage2DInvalidOperation += "1、internalFormat与format不匹配\n";
        glTexImage2DInvalidOperation += "2、如果type是GL_UNSIGNED_SHORT_5_6_5，必须是GL_RGB!\n";
        glTexImage2DInvalidOperation += "3、如果type是GL_UNSIGNED_SHORT_4_4_4_4或GL_UNSIGNED_SHORT_5_5_5_1，必须是GL_RGBA!\n";



        tmpErrorInfoES2["glTexImage2D"] = MTGLDebugCoreAPIInfo("glTexImage2D",
            {"target", "level", "internalformat", "width", "height", "border", "format", "type", "pixels"},
            {kGLEnumType, kGLIntType, kGLIntType, kGLSizeiType, kGLSizeiType, kGLIntType, kGLEnumType, kGLEnumType, kGLVoidPtrType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {glTexImage2DInvalidEnum, glTexImage2DInvalidValue, glTexImage2DInvalidOperation}));

        //        glDeleteTextures
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.


        tmpErrorInfoES2["glDeleteTextures"] = MTGLDebugCoreAPIInfo("glDeleteTextures",
            {"n", "textures"},
            {kGLSizeiType, kConstGLUIntPtr},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glGenFramebuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.
        tmpErrorInfoES2["glGenFramebuffers"] = MTGLDebugCoreAPIInfo("glGenFramebuffers",
            {"n", "framebuffers"},
            {kGLSizeiType, kGLUIntPtrType},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glBindFramebuffer
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.

        tmpErrorInfoES2["glBindFramebuffer"] = MTGLDebugCoreAPIInfo("glBindFramebuffer",
            {"target", "framebuffer"},
            {kGLEnumType, kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"target参数非GL_FRAMEBUFFER!\n"}));

        //        glFramebufferTexture2D
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_FRAMEBUFFER.
        //        GL_INVALID_ENUM is generated if textarget is not an accepted texture target and texture is not 0.
        //        GL_INVALID_ENUM is generated if attachment is not an accepted attachment point.
        //        GL_INVALID_VALUE is generated if level is not 0 and texture is not 0.
        //        GL_INVALID_OPERATION is generated if the default framebuffer object name 0 is bound.
        //        GL_INVALID_OPERATION is generated if texture is neither 0 nor the name of an existing texture object.
        //        GL_INVALID_OPERATION is generated if texture is the name of an existing two-dimensional texture object but textarget is not GL_TEXTURE_2D or if texture is the name of an existing cube map texture object but textarget is GL_TEXTURE_2D.


        std::string glFramebufferTexture2DInvalidEnum = "这个错误有以下三种情况:\n";
        glFramebufferTexture2DInvalidEnum += "1、target非GL_FRAMEBUFFER\n";
        glFramebufferTexture2DInvalidEnum += "2、texture非0的情况下，textarget非可接受的texture target\n";
        glFramebufferTexture2DInvalidEnum += "3、attachment不是一个可接受的值\n";

        std::string glFramebufferTexture2DInvalidValue = "texture非0的情况下，level不等于0！\n";

        std::string glFramebufferTexture2DInvalidOperation = "这个错误有以下三种情况:\n";
        glFramebufferTexture2DInvalidOperation += "1、当前绑定的framebuffer是id为0\n";
        glFramebufferTexture2DInvalidOperation += "2、texture非0的情况下并且找不到任何与之匹配的texture\n";
        glFramebufferTexture2DInvalidOperation += "3、texture是一个二维纹理对象但是texTarget非GL_TEXTURE_2D,或者纹理是一个cube map但是texTarget是GL_TEXTURE_2D\n";



        tmpErrorInfoES2["glFramebufferTexture2D"] = MTGLDebugCoreAPIInfo("glFramebufferTexture2D",
            {"target", "attachment", "textarget", "texture", " level"},
            {kGLEnumType, kGLEnumType, kGLEnumType, kGLUIntType, kGLIntType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {glFramebufferTexture2DInvalidEnum, glFramebufferTexture2DInvalidValue, glFramebufferTexture2DInvalidOperation}));

        //        glDeleteFramebuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.

        tmpErrorInfoES2["glDeleteFramebuffers"] = MTGLDebugCoreAPIInfo("glDeleteFramebuffers",
            {"n", "framebuffers"},
            {kGLSizeiType, kConstGLUIntPtr},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glGenBuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.
        tmpErrorInfoES2["glGenBuffers"] = MTGLDebugCoreAPIInfo("glGenBuffers",
            {"n", "buffers"},
            {kGLSizeiType, kGLUIntPtrType},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glBindBuffer
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not one of the allowable values.

        tmpErrorInfoES2["glBindBuffer"] = MTGLDebugCoreAPIInfo("glBindBuffer",
            {"target", "buffer"},
            {kGLEnumType, kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"target不是一个有效的值\n"}));

        //        glBufferData
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
        //        GL_INVALID_ENUM is generated if usage is not GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW.
        //        GL_INVALID_VALUE is generated if size is negative.
        //        GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.
        //        GL_OUT_OF_MEMORY is generated if the GL is unable to create a data store with the specified size.
        std::string glBufferDataInvalidEnum = "这个错误有以下两种情况:\n";
        glBufferDataInvalidEnum += "1、target非GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER\n";
        glBufferDataInvalidEnum += "2、usage非GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW\n";

        std::string glBufferDataInvalidValue = "size参数为负数";
        std::string glBufferDataInvalidOperation = "target当前绑定的对象为0";
        std::string glBufferDataOutOfMemory = "GL无法按照指定的size创建内存";



        tmpErrorInfoES2["glBufferData"] = MTGLDebugCoreAPIInfo("glBufferData",
            {"target", "size", "data", "usage"},
            {kGLEnumType, kGLUIntPtrType, kGLVoidPtrType, kGLEnumType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION, GL_OUT_OF_MEMORY},
                {glBufferDataInvalidEnum, glBufferDataInvalidValue, glBufferDataInvalidOperation, glBufferDataOutOfMemory}));

        //        glBufferSubData
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.
        //        GL_INVALID_VALUE is generated if offset or size is negative, or if together they define a region of memory that extends beyond the buffer object's allocated data store.
        //        GL_INVALID_OPERATION is generated if the reserved buffer object name 0 is bound to target.


        tmpErrorInfoES2["glBufferSubData"] = MTGLDebugCoreAPIInfo("glBufferSubData",
            {"target", "offset", "size", "data"},
            {kGLEnumType, kGLIntPtrType, kGLSizeiPtrType, kConstGLVoidPtrType},
            convenienceCreateList({GL_INVALID_VALUE, GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {"target非GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER", "offset or size为负数或者两者相定义的区域超出了当前buffer对象分配的内存空间。", "当前target的buffer对象为0"}));

        //        glDeleteBuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.
        tmpErrorInfoES2["glDeleteBuffers"] = MTGLDebugCoreAPIInfo("glDeleteBuffers",
            {"n", "buffers"},
            {kGLSizeiType, kConstGLUIntPtr},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glGenRenderbuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.
        tmpErrorInfoES2["glGenRenderbuffers"] = MTGLDebugCoreAPIInfo("glGenRenderbuffers",
            {"n", "renderbuffers"},
            {kGLSizeiType, kGLUIntPtrType},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glBindRenderbuffer
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_RENDERBUFFER.

        tmpErrorInfoES2["glBindRenderbuffer"] = MTGLDebugCoreAPIInfo("glBindRenderbuffer",
            {"target", "renderbuffer"},
            {kGLEnumType, kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"target非GL_RENDERBUFFER\n"}));

        //        glDeleteRenderbuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if n is negative.
        tmpErrorInfoES2["glDeleteRenderbuffers"] = MTGLDebugCoreAPIInfo("glDeleteRenderbuffers",
            {"n", "renderbuffers"},
            {kGLSizeiType, kConstGLUIntPtr},
            convenienceCreateList({GL_INVALID_VALUE}, {"第一个参数为负数!\n"}));

        //        glDeleteRenderbuffers
        //        Errors
        //        GL_INVALID_VALUE is generated if program is not a value generated by OpenGL.
        //        GL_INVALID_OPERATION is generated if program is not a program object.


        tmpErrorInfoES2["glLinkProgram"] = MTGLDebugCoreAPIInfo("glLinkProgram",
            {"program"},
            {kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {"program非OpenGL生成.\n", "program不是一个program对象\n"}));

        //        glUseProgram
        //        Errors
        //        GL_INVALID_VALUE is generated if program is neither 0 nor a value generated by OpenGL.
        //        GL_INVALID_OPERATION is generated if program is not a program object.
        //        GL_INVALID_OPERATION is generated if program could not be made part of current state.

        std::string glUseProgramInvalidOperation = "这个错误有以下两种情况:\n";
        glUseProgramInvalidOperation += "1、program不是一个program对象\n";


        //        When a program object has been successfully linked, the program object can be made part of current state by calling glUseProgram.
        glUseProgramInvalidOperation += "2、需要先调用glLinkProgram函数.(When a program object has been successfully linked, the program object can be made part of current state by calling glUseProgram.)\n";
        tmpErrorInfoES2["glUseProgram"] = MTGLDebugCoreAPIInfo("glUseProgram",
            {"program"},
            {kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {"program非0也不是OpenGL生成.\n", glUseProgramInvalidOperation}));


        //        glDeleteProgram
        //        Errors
        //        GL_INVALID_VALUE is generated if program is not a value generated by OpenGL.

        tmpErrorInfoES2["glDeleteProgram"] = MTGLDebugCoreAPIInfo("glDeleteProgram",
            {"program"},
            {kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"program非OpenGL生成.\n"}));

        //        glEnableVertexAttribArray
        //        Errors
        //        GL_INVALID_VALUE is generated if index is greater than or equal to GL_MAX_VERTEX_ATTRIBS.

        tmpErrorInfoES2["glEnableVertexAttribArray"] = MTGLDebugCoreAPIInfo("glEnableVertexAttribArray",
            {"index"},
            {kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"index参数大于等于GL_MAX_VERTEX_ATTRIBS.\n"}));
        tmpErrorInfoES2["glDisableVertexAttribArray"] = MTGLDebugCoreAPIInfo("glDisableVertexAttribArray",
            {"index"},
            {kGLUIntType},
            convenienceCreateList({GL_INVALID_VALUE}, {"index参数大于等于GL_MAX_VERTEX_ATTRIBS.\n"}));
        ;

        //        glVertexAttribPointer
        //        Errors
        //        GL_INVALID_ENUM is generated if type is not an accepted value.
        //        GL_INVALID_VALUE is generated if index is greater than or equal to GL_MAX_VERTEX_ATTRIBS.
        //        GL_INVALID_VALUE is generated if size is not 1, 2, 3, or 4.
        //        GL_INVALID_VALUE is generated if stride is negative.
        std::string glVertexAttribPointerInvalidValue = "这个错误有以下三种情况:\n";
        glVertexAttribPointerInvalidValue += "1、index大于等于GL_MAX_VERTEX_ATTRIBS\n";
        glVertexAttribPointerInvalidValue += "2、size非1、2、3或4\n";
        glVertexAttribPointerInvalidValue += "3、stride是负数\n";



        tmpErrorInfoES2["glVertexAttribPointer"] = MTGLDebugCoreAPIInfo("glVertexAttribPointer",
            {"indx", "size", "type", "normalized", "stride", "ptr"},
            {kGLUIntType, kGLIntType, kGLEnumType, kGLBooleanType, kGLSizeiType, kConstGLVoidPtrType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE},
                {"type不是一个可接受的值.\n", glVertexAttribPointerInvalidValue}));

        //        glUniform
        //        Errors
        //        GL_INVALID_OPERATION is generated if there is no current program object.
        //        GL_INVALID_OPERATION is generated if the size of the uniform variable declared in the shader does not match the size indicated by the glUniform command.
        //        GL_INVALID_OPERATION is generated if one of the integer variants of this function is used to load a uniform variable of type float, vec2, vec3, vec4, or an array of these, or if one of the floating-point variants of this function is used to load a uniform variable of type int, ivec2, ivec3, or ivec4, or an array of these.
        //        GL_INVALID_OPERATION is generated if location is an invalid uniform location for the current program object and location is not equal to -1.
        //        GL_INVALID_VALUE is generated if count is less than 0.
        //        GL_INVALID_VALUE is generated if transpose is not GL_FALSE.
        //        GL_INVALID_OPERATION is generated if count is greater than 1 and the indicated uniform variable is not an array variable.
        //        GL_INVALID_OPERATION is generated if a sampler is loaded using a command other than glUniform1i and glUniform1iv.


        std::string glUniformInvalidValue = "这个错误有以下两种情况:\n";
        glUniformInvalidValue += "1、count参数小于0\n";
        glUniformInvalidValue += "2、transpose值非GL_FALSE\n";

        std::string glUniformInvalidOperation = "这个错误有以下六种情况:\n";
        glUniformInvalidOperation += "1、它并非当前program的对象。\n";
        glUniformInvalidOperation += "2、size与shader声明的不一致\n";
        glUniformInvalidOperation += "3、类型不匹配如：integer类型传入了脚本type为float, vec2, vec3, vec4\n";
        glUniformInvalidOperation += "4、location是一个无效的值\n";
        glUniformInvalidOperation += "5、count大于1但uniform非array变量\n";
        glUniformInvalidOperation += "6、sampler类型必须通过glUniform1i and glUniform1iv设置.\n";

        auto tmpError = convenienceCreateList({GL_INVALID_OPERATION, GL_INVALID_VALUE},
            {glUniformInvalidOperation, glUniformInvalidValue});


        tmpErrorInfoES2["glUniform1i"] = MTGLDebugCoreAPIInfo("glUniform1i",
            {"location", "x"},
            {kGLIntType, kGLIntType},
            tmpError);
        ;
        tmpErrorInfoES2["glUniformMatrix4fv"] = MTGLDebugCoreAPIInfo("glUniformMatrix4fv",
            {"location", "count", "transpose", "value"},
            {kGLIntType, kGLSizeiType, kGLBooleanType, kGLFloatPtrType},
            tmpError);
        tmpErrorInfoES2["glUniform2f"] = MTGLDebugCoreAPIInfo("glBindTexture",
            {"location", "x", "y"},
            {kGLIntType, kGLFloatType, kGLFloatType},
            tmpError);
        //        glDrawElements
        //        Errors
        //        GL_INVALID_ENUM is generated if mode is not an accepted value.
        //        GL_INVALID_ENUM is generated if type is not GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT.
        //        GL_INVALID_VALUE is generated if count is negative.
        //        GL_INVALID_FRAMEBUFFER_OPERATION is generated if the currently bound framebuffer is not framebuffer complete (i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE).

        std::string glDrawElementsInvalidEnum = "这个错误有以下两种情况:\n";
        glDrawElementsInvalidEnum += "1、mode不是一个可接受的值\n";
        glDrawElementsInvalidEnum += "2、type非GL_UNSIGNED_BYTE or GL_UNSIGNED_SHORT\n";

        tmpErrorInfoES2["glDrawElements"] = MTGLDebugCoreAPIInfo("glDrawElements",
            {"mode", "count", "type", "indices"},
            {kGLEnumType, kGLSizeiType, kGLEnumType, kConstGLVoidPtrType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_FRAMEBUFFER_OPERATION},
                {glDrawElementsInvalidEnum, "count是负数", "当前绑定的framebuffer不是一个完整的framebuffer(i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE)"}));


        tmpErrorInfoES2["glDrawArrays"] = MTGLDebugCoreAPIInfo("glDrawArrays",
            {"mode", "first", "count"},
            {kGLEnumType, kGLIntType, kGLSizeiType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_FRAMEBUFFER_OPERATION},
                {"mode不是一个可接受的值", "count是负数", "当前绑定的framebuffer不是一个完整的framebuffer(i.e. the return value from glCheckFramebufferStatus is not GL_FRAMEBUFFER_COMPLETE)"}));

        //        glClear
        //        Errors
        //        GL_INVALID_VALUE is generated if any bit other than the three defined bits is set in mask.



        tmpErrorInfoES2["glClear"] = MTGLDebugCoreAPIInfo("glClear",
            {"mask"},
            {kGLBitfieldType},
            convenienceCreateList({GL_INVALID_VALUE, GL_INVALID_FRAMEBUFFER_OPERATION},
                {"mask参数不是默认定义的三个类型\n", "clear操作的Framebuffer不完整或为空"}));

        //        glTexSubImage2D
        //        Errors
        //        GL_INVALID_ENUM is generated if target is not GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or GL_TEXTURE_CUBE_MAP_NEGATIVE_Z.
        //      GL_INVALID_ENUM is generated if format or type is not an accepted value.
        //      GL_INVALID_VALUE is generated if level is less than 0.
        //      GL_INVALID_VALUE may be generated if level is greater than log 2 max, where max is the returned value of GL_MAX_TEXTURE_SIZE.
        //      GL_INVALID_VALUE is generated if xoffset < 0 , xoffset + width > w , yoffset < 0 , or yoffset + height > h , where w is the width and h is the height of the texture image being modified.
        //      GL_INVALID_VALUE is generated if width or height is less than 0.
        //      GL_INVALID_OPERATION is generated if the texture array has not been defined by a previous glTexImage2D or glCopyTexImage2D operation whose internalformat matches the format of glTexSubImage2D.
        //      GL_INVALID_OPERATION is generated if type is GL_UNSIGNED_SHORT_5_6_5 and format is not GL_RGB.
        //      GL_INVALID_OPERATION is generated if type is GL_UNSIGNED_SHORT_4_4_4_4 or GL_UNSIGNED_SHORT_5_5_5_1 and format is not GL_RGBA.
        std::string glTexSubImage2DInvalidEnum = "这个错误有以下三种情况:\n";
        glTexSubImage2DInvalidEnum += "1、参数target不是以下这些GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, or GL_TEXTURE_CUBE_MAP_NEGATIVE_Z \n";
        glTexSubImage2DInvalidEnum += "2、参数format不是 GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA \n";
        glTexSubImage2DInvalidEnum += "3、参数type不是 GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1 \n";

        std::string glTexSubImage2DInvalidValue = "这个错误有以下两种情况:\n";
        glTexSubImage2DInvalidValue += "1、level小于0 或 level大于log2（最大纹理限制尺寸）\n";
        glTexSubImage2DInvalidValue += "2、width或者height小于0; 可能xoffset小于0 ,xoffset + width > 当前要修改的纹理宽度;可能yoffset小于0 ,yoffset + height > 当前要修改的纹理高度";

        std::string glTexSubImage2DInvalidOperation = "这个错误有以下三种情况:\n";
        glTexSubImage2DInvalidOperation += "1、没有绑定要处理的纹理(且这个纹理的internalformat要和glTexSubImage2D的format一致)\n";
        glTexSubImage2DInvalidOperation += "2、type GL_UNSIGNED_SHORT_5_6_5 对应的 format 不是 GL_RGB. 或者 type GL_UNSIGNED_SHORT_4_4_4_4 or GL_UNSIGNED_SHORT_5_5_5_1 对应的 format 不是 GL_RGBA.\n";


        tmpErrorInfoES2["glTexSubImage2D"] = MTGLDebugCoreAPIInfo("glBindTexture",
            {"target", "level", "xoffset", "yoffset", "width", "height", "format", "type", "pixels"},
            {kGLEnumType, kGLIntType, kGLIntType, kGLIntType, kGLSizeiType, kGLSizeiType, kGLEnumType, kGLEnumType, kConstGLVoidPtrType},
            convenienceCreateList({GL_INVALID_ENUM, GL_INVALID_VALUE, GL_INVALID_OPERATION},
                {glTexSubImage2DInvalidEnum, glTexSubImage2DInvalidValue, glTexSubImage2DInvalidOperation}));


#ifdef MTGLDEBUG_PLATFORM_ANDROID

        tmpErrorInfoES2["eglCreateContext"] = MTGLDebugCoreAPIInfo("eglCreateContext",
            {"dpy", "config", "share_context", "attrib_list"},
            {kEGLDisplayType, kEGLConfigType, kEGLContextType, kConstEGLintPtrType},
            std::map<GLenum, std::string>());
#endif

        std::map<std::string, MTGLDebugCoreAPIInfo> tmpErrorInfoES3 = tmpErrorInfoES2;


        errorInfo[MTGLDebugCoreGLAPIVersionGLES2] = tmpErrorInfoES2;
        errorInfo[MTGLDebugCoreGLAPIVersionGLES3] = tmpErrorInfoES3;
    });
    return errorInfo;
}

void MTGLDebugCore::exception(const char *methodName, unsigned int error) {

    if (!MTGLDebugCore::isEnable()) {
        return;
    }

    // Default is use ES2
    MTGLDebugCoreGLAPIVersion api = currentGLAPIVersion();
    std::map<MTGLDebugCoreGLAPIVersion, std::map<std::string, MTGLDebugCoreAPIInfo>> glErrorInfoApiMap = GetErrorInfo();
    std::map<std::string, MTGLDebugCoreAPIInfo> glErrorInfoMap = glErrorInfoApiMap[api];
    MTGLDebugCoreAPIInfo glFunctionError = glErrorInfoMap[methodName];
    std::string errorString = glFunctionError.errorWithType(error);
    errorString = errorString.empty() ? "GLError.\n" : errorString;
    std::string exception = "ErrorAPI:" + std::string(methodName) + "\n" + errorString + "ErrorCode:" + std::to_string(error);
    GLDebug::_throw(exception, MTGLDebugCoreErrorTypeWarning);
}
