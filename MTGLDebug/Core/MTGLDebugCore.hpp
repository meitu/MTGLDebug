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


#ifndef MTGLDebugCore_hpp
#define MTGLDebugCore_hpp

#include <functional>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "MTGLDebugCoreObject.hpp"
#include "MTGLDebugQueue.hpp"
#include "MTGLDebug_Macro.h"
#include "MTGLDebug_Platform.h"

extern const std::type_info *kGLEnumType;
extern const std::type_info *kGLUIntPtrType;
extern const std::type_info *kGLUIntType;
extern const std::type_info *kGLSizeiType;
extern const std::type_info *kGLIntType;
extern const std::type_info *kGLVoidPtrType;
extern const std::type_info *kConstGLUIntPtr;
extern const std::type_info *kGLSizeiPtrType;
extern const std::type_info *kConstGLVoidPtrType;
extern const std::type_info *kGLIntPtrType;
extern const std::type_info *kGLBooleanType;
extern const std::type_info *kGLFloatType;
extern const std::type_info *kGLFloatPtrType;
extern const std::type_info *kGLBitfieldType;

#ifdef MTGLDEBUG_PLATFORM_ANDROID
extern const std::type_info *kEGLDisplayType;
extern const std::type_info *kEGLConfigType;
extern const std::type_info *kEGLContextType;
extern const std::type_info *kConstEGLintPtrType;

#include <jni.h>


#endif


/*
 originDebugObject
 不为空 正确
 不存在 不正确
 不存在的情况：
 
 MTGLDebugObjectStatusUseWronglyGLObject
 1、MTGLDebugObjectStatusUseWronglyOperateGLObject
 上下文A存在id为1的纹理texture1, 然后在上下文B中操作id为1的texture,导致当前上下文不存在该对象,历史记录中上下文B不存在该对象(上下文绑定不当,操作了另外一上下文的对象)
 2、MTGLDebugObjectStatusUseWronglyDeleteGLObject
 上下文A存在id为1的纹理texture1, 然后在上下文B中操作id为1的texture,导致当前上下文B不存在该对象,历史记录中上下文B存在该对象。对不同上下文相同id(或可能误删除的)GL对象操作。
 
 MTGLDebugObjectStatusUseForwardlyDeletedGLObject 另外一个上下文创建，不知情(主动)被删除;
 MTGLDebugObjectStatusNotExistGLObject  不存在的,未被创建过。（开发者不知道）
 */

namespace GLDebug {
#ifdef __cplusplus
extern "C" {
#endif
extern const void *currentContext();
extern const void *currentSharegroup();
#ifdef __cplusplus
}
#endif

typedef std::map<std::string, std::map<std::string, const MTGLDebugCoreObject *> *> MTGLDebugImmutableSharegroupsMap;
typedef std::map<std::string, std::map<std::string, MTGLDebugCoreObject *> *> MTGLDebugSharegroupsMap;
typedef std::map<std::string, const MTGLDebugCoreObject *> MTGLDebugCoreObjectImmutableMap;
typedef std::map<std::string, MTGLDebugCoreObject *> MTGLDebugCoreObjectMap;

enum MTGLDebugObjectStatus {
    MTGLDebugObjectStatusNotExistGLObject,            //一直没有被创建
    MTGLDebugObjectStatusUseForwardlyDeletedGLObject, //未知情况被主动删除

    MTGLDebugObjectStatusUseWronglyDeleteGLObject,  //对不同上下文相同id(因误删除的)GL对象操作
    MTGLDebugObjectStatusUseWronglyOperateGLObject, //对不同上下文相同id的GL对象操作
    MTGLDebugObjectStatusUseWronglyGLObject         //错误操作包含上面两种
};

enum MTGLDebugCoreErrorType {
    MTGLDebugCoreErrorTypeError,
    MTGLDebugCoreErrorTypeWarning
};

enum MTGLDebugCoreGLAPIVersion {
    MTGLDebugCoreGLAPIVersionGLES1 = 1,
    MTGLDebugCoreGLAPIVersionGLES2 = 2,
    MTGLDebugCoreGLAPIVersionGLES3 = 3,
};

enum MTGLDebugCoreOptions {

    // 完全关闭MTGLDebug
    MTGLDebugCoreOptionsDisable = 1UL << 0,

    // 关闭异常报错
    MTGLDebugCoreOptionsDisableException = 1UL << 1,

    // 使用状态检测
    MTGLDebugCoreOptionsCheckAPIUsageStates = 1UL << 2,

    // 上下文正确性检测
    MTGLDebugCoreOptionsCheckContextStates = 1UL << 3,

    // 仅统计GL对象内存
    MTGLDebugCoreOptionsOnlyStatisticalGLObject = 1UL << 4,
};


struct MTGLDebugCoreInputObject {
    GLuint object;
    GLenum target;
    int initClass;

    MTGLDebugCoreInputObject() = default;
    MTGLDebugCoreInputObject(const MTGLDebugCoreInputObject &obj) {
        object = obj.object;
        target = obj.target;
        initClass = obj.initClass;
    }
};

struct MTGLDebugCoreAPIInfo {
    std::vector<std::string> paramNames;
    std::vector<const std::type_info *> paramTypes;
    std::map<GLenum, std::string> errors;
    std::string apiName;

    MTGLDebugCoreAPIInfo() = default;

    MTGLDebugCoreAPIInfo(const std::string &nName,
        const std::vector<std::string> &nParamNames,
        const std::vector<const std::type_info *> &nParamTypes,
        const std::map<GLenum, std::string> &nErrors) {
        this->paramNames = nParamNames;
        this->paramTypes = nParamTypes;
        this->errors = nErrors;
        this->apiName = nName;
    }

    MTGLDebugCoreAPIInfo(const MTGLDebugCoreAPIInfo &info) {
        paramNames = info.paramNames;
        paramTypes = info.paramTypes;
        errors = info.errors;
        apiName = info.apiName;
    }

    std::string errorWithType(GLenum type) {
        return errors[type];
    }
};


class MTGLDebugCore
{

  public:
    /**
		 已使用OpenGLES资源占用内存大小(单位：字节)
		 
		 return 已使用OpenGLES资源占用内存大小(单位：字节)
		 */
    static size_t fetchOpenGLESResourceMemorySize(void);

    /**
		 异步获取已使用OpenGLES资源占用内存大小(单位：字节)
		 
		 @param completionHandler 完成回调
		 */
    static void fetchOpenGLESResourceMemorySizeAsynchronously(std::function<void(size_t size)> completionHandler);

    /**
		 传入sharegroup
		 
		 return sharegroup已使用OpenGLES资源占用内存大小(单位：字节)
		 */
    static size_t fetchEAGLSharegroupMemorySize(void *sharegroup);


    /**
		 按内存使用大小输出排序后的MTGLDebugObject对象
		 
		 数据可以通过对象进行查看
		 
		 return 按内存使用大小输出排序后的
		 */
    static std::vector<MTGLDebugCoreObject *> sortedDebugObjectsByMemory(void);

    /**
		 异步获取按内存使用大小输出排序后的MTGLDebugObject对象
		 
		 @param completionHandler 完成回调
		 */
    static void sortedDebugObjectsByMemoryAsynchronously(std::function<void(std::vector<MTGLDebugCoreObject *> result)> completionHandler);



    /**
		 每一秒调用drawCall次数
		 
		 return 结果
		 */
    static unsigned int GetDrawCallsPerSeconds(void) {
        return MTGLDebugCore::sharedMTGLDebugCore()->drawCallsPerSeconds();
    }

    static bool isEnable() {
        return !(MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions & MTGLDebugCoreOptionsDisable);
    }
    static void SetGLDebugEnable(bool enable);

    static bool isEnableGLDebugException() {
        return !(MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions & MTGLDebugCoreOptionsDisableException);
    }
    static void SetGLDebugException(bool enable);

    static void SetGLDebugErrorType(MTGLDebugCoreErrorType type) {
        MTGLDebugCore::sharedMTGLDebugCore()->mDebugErrorType = type;
    }

    static MTGLDebugCoreErrorType GLDebugErrorType() {
        return MTGLDebugCore::sharedMTGLDebugCore()->mDebugErrorType;
    }
    static bool isEnableCheckAPIUsageStates() {
        return (MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions & MTGLDebugCoreOptionsCheckAPIUsageStates);
    }

    static void SetGLDebugOnlyStatisticalGLObject(bool enable);

    static bool isEnableOnlyStatisticalGLObject() {
        return (MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions & MTGLDebugCoreOptionsOnlyStatisticalGLObject);
    }


    static const MTGLDebugCoreOptions &debugOptions() {
        return MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions;
    }

    static MTGLDebugCoreOptions defaultDebugOptions() {
        return (MTGLDebugCoreOptions)(MTGLDebugCoreOptionsCheckAPIUsageStates | MTGLDebugCoreOptionsCheckContextStates);
    }

    static void SetDebugOptions(MTGLDebugCoreOptions options) {
        MTGLDebugCore::sharedMTGLDebugCore()->mDebugOptions = options;
    }


    static bool checkObjectPublic(const MTGLDebugCoreInputObject &inputObject,
        MTGLDebugCoreObjectMap **outputShareGroupObjects, unsigned int *outputErrorType) {
        return MTGLDebugCore::sharedMTGLDebugCore()->checkObject(inputObject, outputShareGroupObjects, outputErrorType);
    }

    static MTGLDebugCoreObject *addObjectPublic(const MTGLDebugCoreInputObject &inputObj, size_t memorySize) {
        return MTGLDebugCore::sharedMTGLDebugCore()->addObject(inputObj, memorySize);
    }

#ifdef MTGLDEBUG_PLATFORM_APPLE
    static void addCVObjectPublic(const MTGLDebugCoreInputObject &parentInputObject, size_t parentMemorySize,
        const MTGLDebugCoreInputObject &childInputObject, size_t childMemorySize) {
        MTGLDebugCore::sharedMTGLDebugCore()->addCVObject(parentInputObject, parentMemorySize, childInputObject, childMemorySize);
    }
#endif


    static bool deleteObjectPublic(const MTGLDebugCoreInputObject &inputObject) {
        return MTGLDebugCore::sharedMTGLDebugCore()->deleteObject(inputObject);
    }

    static void deleteObjectsPublic(int count, const unsigned int *params, unsigned int target, int initClass) {
        MTGLDebugCore::sharedMTGLDebugCore()->deleteObjectsFunc(count, params, target, initClass);
    }

    static void didDrawPublic() {
        MTGLDebugCore::sharedMTGLDebugCore()->didDraw();
    }

    static MTGLDebugImmutableSharegroupsMap GetImmutableSharegroups(void) {
        return MTGLDebugCore::sharedMTGLDebugCore()->immutableSharegroups();
    }

    static MTGLDebugSharegroupsMap *GetSharegroups(void) {
        return MTGLDebugCore::sharedMTGLDebugCore()->sharegroups();
    }

    static void runAsyncBlockInTheSafeThread(std::function<void()> block);
    static void runSyncBlockInTheSafeThread(std::function<void()> block);

    static MTGLDebugCore *sharedMTGLDebugCore(void);

    static void exceptionPublic(const char *methodName, unsigned int error) {
        MTGLDebugCore::sharedMTGLDebugCore()->exception(methodName, error);
    }

    static std::map<MTGLDebugCoreGLAPIVersion, std::map<std::string, MTGLDebugCoreAPIInfo>> &GetErrorInfoPublic() {
        return MTGLDebugCore::sharedMTGLDebugCore()->GetErrorInfo();
    };

    void willCallAPI(const char *methodName, ...);
    void didCallAPI(const char *methodName, const void *result, ...);

    /**
		 根据上下文context地址获取shareGroupID
		 
		 @return 返回上下文所属sharegroup字符串
		 */
    static std::string sharegroupIDWithAddress(const void *contextShapegoup);

    static MTGLDebugCoreObject *fetchDebugObjectPublic(const void *contextShapegoup, GLenum target, unsigned int object) {
        return MTGLDebugCore::sharedMTGLDebugCore()->fetchDebugObject(contextShapegoup, target, object);
    };

    void flushMTGLDebugCacheObject();

    static const void *currentContextSharegourpPublic(void) {
        return currentSharegroup();
    }

    static const void *currentContextPublic(void) {
        return currentContext();
    }

    static MTGLDebugCoreGLAPIVersion currentGLAPIVersionPublic(void) {
        return MTGLDebugCore::sharedMTGLDebugCore()->currentGLAPIVersion();
    }

    static GLDebug::Queue *debugQueue(void);

    MTGLDebugCore();

    virtual ~MTGLDebugCore();

    std::function<void(std::string errorMes, MTGLDebugCoreErrorType type)> errorMessageHandler = nullptr;

#ifdef MTGLDEBUG_PLATFORM_ANDROID
    static void initSharedGLDebugObject(JNIEnv *env);
#endif

  protected:
    MTGLDebugCoreObject *cacheObject = NULL;

    std::string mProjectName;
    std::vector<double> *mDrawCallsPerSeconds = NULL;

    GLDebug::Queue *mDebugQueue = NULL;

    MTGLDebugCoreErrorType mDebugErrorType = MTGLDebugCoreErrorTypeWarning;
    MTGLDebugCoreOptions mDebugOptions = MTGLDebugCore::defaultDebugOptions();
    MTGLDebugSharegroupsMap *mShareGroup = NULL;
    //所有上下文的历史记录,只增加不减少,目的是让开发者知道GL对象是否在未知情的情况下被其他主动删除
    MTGLDebugSharegroupsMap *mHistoryShareGroups = NULL;

    MTGLDebugCoreObjectMap *getLazyShareGroupObjects(MTGLDebugSharegroupsMap *sharegroups, std::string sharegroupID);
    MTGLDebugCoreObjectMap *getLazyHistorySharegroupObjects(MTGLDebugSharegroupsMap *historySharegroups, std::string sharegroupID);

    unsigned int drawCallsPerSeconds(void);


    /**
		 移除托管对象
		 */
    virtual bool deleteObject(const MTGLDebugCoreInputObject &inputObject);
    void deleteObjectsFunc(int count, const unsigned int *params, unsigned int target, int initClass);


    /**
		 检查当前对象是否在当前sharegroup中
		 
		 @param inputObject 输入对象
		 */

    virtual bool checkObject(const MTGLDebugCoreInputObject &inputObject,
        MTGLDebugCoreObjectMap **outputShareGroupObjects,
        unsigned int *outputErrorType,
        bool isDelete = false,
        const void *context = currentContext(),
        const void *sharegroup = currentSharegroup());



    /**
         添加托管对象
         @param inputObject 对象值
         @param memorySize 内存占用
         @return 添加成功后的对象
         */
    virtual MTGLDebugCoreObject *addObject(MTGLDebugCoreInputObject inputObject,
        size_t memorySize);


#ifdef MTGLDEBUG_PLATFORM_APPLE
    void addCVObject(const MTGLDebugCoreInputObject &parentInputObject, size_t parentMemorySize,
        const MTGLDebugCoreInputObject &childInputObject, size_t childMemorySize);
#endif


    /**
		 异常判断
		 */
    void exception(const char *methodName, unsigned int error);


    /**
		 根据上下文和对象ID获取MTGLDebugObject对象
		 */
    virtual MTGLDebugCoreObject *fetchDebugObject(const void *contextShapegoup, GLenum taget, unsigned int object);


    /**
		 返回当前GL版本
		 */

    virtual MTGLDebugCoreGLAPIVersion currentGLAPIVersion(void) = 0;

    /**
		 sharegroups存储所有上下文单例
		 
		 @return 返回当前sharegroups单例
		 */
    MTGLDebugImmutableSharegroupsMap immutableSharegroups(void);
    MTGLDebugImmutableSharegroupsMap immutableHistorySharegroups(void);
    MTGLDebugSharegroupsMap *sharegroups(void);
    MTGLDebugSharegroupsMap *historySharegroups(void);

    void runBlockInTheSafeThread(std::function<void()> block, bool isAsync);

    void didDraw(void);



    std::map<MTGLDebugCoreGLAPIVersion, std::map<std::string, MTGLDebugCoreAPIInfo>> &GetErrorInfo();
};

#ifdef __cplusplus //C++
extern "C" {
#endif

inline void _throw(std::string &errorMessage, MTGLDebugCoreErrorType type) { //inline关键字表示内联函数
    if (MTGLDebugCore::isEnableGLDebugException() && !MTGLDebugCore::isEnableOnlyStatisticalGLObject()) {

#ifdef MTGLDEBUG_PLATFORM_ANDROID
        //调用java层的异常抛出
        MTGLDebugCore_Android::runSyncBlockInTheSafeThread([=] {
            ((MTGLDebugCore_Android *)MTGLDebugCore::sharedMTGLDebugCore())->errorExceptionCallback(errorMessage);
        });
#else

        if (MTGLDebugCore::sharedMTGLDebugCore()->errorMessageHandler) {
            MTGLDebugCore::sharedMTGLDebugCore()->errorMessageHandler(errorMessage, type);
        }

#endif
    }
}
#ifdef __cplusplus
}
#endif
} // namespace GLDebug

#endif /* MTGLDebugCore_hpp */
