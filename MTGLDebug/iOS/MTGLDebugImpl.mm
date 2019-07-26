//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 16/9/21
// Created by: Zed
//


#import "MTGLDebugImpl.h"

#import "MTGLDebugGLFunction.h"
#import "MTGLDebugGLHookFunction.h"
#import "MTGLDebugObject+QuickLook.h"

#include "MTGLDebugCore.hpp"
#include "MTGLDebugCoreObject.hpp"
#include "MTGLDebugCore_iOS.h"
#include "MTGLDebugObject_Internal.h"

using namespace GLDebug;
static id<MTGLDebugErrorMessageDelegate> _delegate = nil;
@implementation MTGLDebug {
    NSString *_projectName;
    NSMutableArray<NSNumber *> *_drawCallsPerSeconds;
}

+ (void)registerMTGLDebugHook {
    mtglDebug_register();
}

+ (void)setDelegate:(id<MTGLDebugErrorMessageDelegate>)delegate {
    std::function<void(std::string errorMes, MTGLDebugCoreErrorType type)> block = [=](std::string errorMes, MTGLDebugCoreErrorType type) {
        if ([delegate respondsToSelector:@selector(glDebugErrorMessageHandler:errorType:)]) {
            NSString *errorStr = [NSString stringWithCString:errorMes.c_str() encoding:NSUTF8StringEncoding];
            [delegate glDebugErrorMessageHandler:errorStr
                                       errorType:(MTGLDebugErrorType)type];
        }
    };
    _delegate = delegate;
    GLDebug::MTGLDebugCore::sharedMTGLDebugCore()->errorMessageHandler = block;
    
}

+ (id<MTGLDebugErrorMessageDelegate>)delegate {
    return _delegate;
}


+ (void)setErrorType:(MTGLDebugErrorType)errorType {
    GLDebug::MTGLDebugCore::SetGLDebugErrorType((MTGLDebugCoreErrorType)errorType);
}
+ (MTGLDebugErrorType)errorType {
    return (MTGLDebugErrorType)GLDebug::MTGLDebugCore::GLDebugErrorType();
}
//#ifdef UNITTEST
+ (BOOL)isEnableGLDebugException {
    return GLDebug::MTGLDebugCore::isEnableGLDebugException();
}

+ (void)setEnableGLDebugException:(BOOL)enableGLDebugException {
    GLDebug::MTGLDebugCore::SetGLDebugException(enableGLDebugException);
}
//#endif

+ (BOOL)isEnableOnlyStatisticalGLObject {
    return GLDebug::MTGLDebugCore::isEnableOnlyStatisticalGLObject();
}

+ (void)setEnableOnlyStatisticalGLObject:(BOOL)enableOnlyStatisticalGLObject {
    GLDebug::MTGLDebugCore::SetGLDebugOnlyStatisticalGLObject(enableOnlyStatisticalGLObject);
    
    if (enableOnlyStatisticalGLObject) {
        [self setEnableGLDebugException:NO];
    } else {
        [self setEnableGLDebugException:YES];
    }
}

+ (void)recordCurrentCallStack:(NSArray<NSString *> *)calls
                  outputResult:(NSMutableArray<NSString *> *__autoreleasing *)outputResult
                operationQueue:(NSOperationQueue *)operationQueue
                    writeQueue:(NSOperationQueue *)writeQueue {
    NSMutableArray<NSString *> *tmpOutputResult = [NSMutableArray array];

    if (outputResult) {
        *outputResult = tmpOutputResult;
    }

    [operationQueue addOperationWithBlock:^{
        NSLog(@"1");
        NSString *firstOCFuncName = @"]";
        NSString *glFunction = @"mtgldebug_";

        NSUInteger start = NSNotFound;
        NSUInteger end = NSNotFound;
        for (NSInteger i = 0; i < calls.count; i++) {
            NSString *obj = start == NSNotFound ? calls[i] : nil;

            if (start == NSNotFound && [obj rangeOfString:glFunction].location != NSNotFound) {
                start = i;
                continue;
            }

            NSUInteger endIndex = calls.count - 1 - i;
            obj = end == NSNotFound ? calls[endIndex] : nil;
            if (end == NSNotFound && [obj rangeOfString:firstOCFuncName].location != NSNotFound) {
                end = endIndex;
            }

            if (start != NSNotFound && end != NSNotFound) {
                break;
            }
        }

        NSRange range = NSMakeRange(start, end - start);

        NSBlockOperation *blockOperation = [NSBlockOperation blockOperationWithBlock:^{
            [tmpOutputResult addObjectsFromArray:[calls objectsAtIndexes:[NSIndexSet indexSetWithIndexesInRange:range]]];
        }];

        [writeQueue addOperation:blockOperation];
    }];
}



- (NSString *)projectName {
    if (!_projectName) {
        NSBundle *mainBundle = [NSBundle mainBundle];
        //项目名称
        NSString *executableFile = [mainBundle objectForInfoDictionaryKey:(NSString *)kCFBundleExecutableKey];
        _projectName = [executableFile copy];
    }
    return _projectName;
}



+ (size_t)fetchOpenGLESResourceMemorySize {
    return GLDebug::MTGLDebugCore_iOS::fetchOpenGLESResourceMemorySize();
}

+ (void)fetchOpenGLESResourceMemorySizeAsynchronously:(void (^)(size_t))completionHandler {
    GLDebug::MTGLDebugCore_iOS::fetchOpenGLESResourceMemorySizeAsynchronously([=](size_t size) -> void {
        completionHandler(size);
    });
}

+ (NSArray<MTGLDebugObject *> *)sortedDebugObjectsByMemory {
    NSMutableArray<MTGLDebugObject *> *sortMemoryUsageObject = [NSMutableArray array];
    std::vector<GLDebug::MTGLDebugCoreObject *> debugCoreObjects = MTGLDebugCore_iOS::sortedDebugObjectsByMemory();
    for (auto items : debugCoreObjects) {
        MTGLDebugObject *object = [MTGLDebugObject covertCplusplusObject:items];
        [sortMemoryUsageObject addObject:object];
        delete items;
    }
    return [sortMemoryUsageObject copy];
}

+ (void)sortedDebugObjectsByMemoryAsynchronously:(void (^)(NSArray<MTGLDebugObject *> *))completionHandler {
    GLDebug::MTGLDebugCore_iOS::sortedDebugObjectsByMemoryAsynchronously([=](std::vector<GLDebug::MTGLDebugCoreObject *> debugCoreObjects) -> void {
        NSMutableArray<MTGLDebugObject *> *sortMemoryUsageObject = [NSMutableArray array];
        for (auto items : debugCoreObjects) {
            MTGLDebugObject *object = [MTGLDebugObject covertCplusplusObject:items];
            [sortMemoryUsageObject addObject:object];
            delete items;
        }
        completionHandler([sortMemoryUsageObject copy]);
    });
}

+ (NSUInteger)drawCallsPerSeconds {
    NSUInteger count = 0;
    count = GLDebug::MTGLDebugCore_iOS::GetDrawCallsPerSeconds();
    return count;
}

+ (void)didDraw {
    GLDebug::MTGLDebugCore_iOS::didDrawPublic();
}

+ (MTGLDebugObject *)debugObjectForContext:(EAGLContext *)context
                                    target:(GLenum)target
                                    object:(GLuint)objectId {
    auto ptr = GLDebug::MTGLDebugCore_iOS::fetchDebugObjectPublic((__bridge void *)context.sharegroup, target, objectId);
    MTGLDebugObject *debugObj = [MTGLDebugObject covertCplusplusObject:ptr];
    return debugObj;
}



@end

@implementation EAGLSharegroup (MTGLDebug)

- (size_t)mtgldebug_memorySize {
    size_t memorySize = GLDebug::MTGLDebugCore_iOS::fetchEAGLSharegroupMemorySize((__bridge void *)self);

    return memorySize;
}

@end
