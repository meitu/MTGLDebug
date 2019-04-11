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


#import <Foundation/Foundation.h>
#import "MTGLDebugObject.h"

typedef NS_ENUM(NSUInteger, MTGLDebugErrorType) {
    MTGLDebugErrorTypeError,
    MTGLDebugErrorTypeWarning,
};
@protocol MTGLDebugErrorMessageDelegate <NSObject>

@optional
- (void)glDebugErrorMessageHandler:(NSString *)errorMessage errorType:(MTGLDebugErrorType)type;

@end

/**
 项目接入要求：在debug的情况下可以帮助排查问题，release需要注意不要引入使用。
 */
@interface MTGLDebug : NSObject

/**
 注册hook函数
 */

+ (void)registerMTGLDebugHook;

@property (nonatomic, weak, class) id<MTGLDebugErrorMessageDelegate> delegate;

//@property (nonatomic, assign, class) MTGLDebugErrorType errorType;

/**
 设置
 YES 仅统计GL对象的内存
 NO 内存统计、API检测和上下文逻辑分析
 
 */
@property (nonatomic, assign, class, getter=isEnableOnlyStatisticalGLObject) BOOL enableOnlyStatisticalGLObject;

///**
// 设置是否对GLAPI进行getError错误判断【十分耗性能】
// */
//@property (nonatomic, assign, class, getter=isEnableCheckAPIUsageStates) BOOL enableCheckAPIUsageStates;
//
///**
// 设置是否对上下文资源进行逻辑检测【耗性能】
// */
//@property (nonatomic, assign, class, getter=isEnableCheckContextStates) BOOL enableCheckContextStates;
//


/**
 已使用OpenGLES资源占用内存大小(单位：字节)

 @return 已使用OpenGLES资源占用内存大小(单位：字节)
 */
+ (size_t)fetchOpenGLESResourceMemorySize;

/**
 异步获取已使用OpenGLES资源占用内存大小(单位：字节)

 @param completionHandler 完成回调
 */
+ (void)fetchOpenGLESResourceMemorySizeAsynchronously:(void (^)(size_t size))completionHandler;

/**
 按内存使用大小输出排序后的MTGLDebugObject对象
 
 数据可以通过对象进行查看
 
 @return 按内存使用大小输出排序后的
 */
+ (NSArray<MTGLDebugObject *> *)sortedDebugObjectsByMemory;

/**
 异步获取按内存使用大小输出排序后的MTGLDebugObject对象
 
 @param completionHandler 完成回调
 */
+ (void)sortedDebugObjectsByMemoryAsynchronously:(void (^)(NSArray<MTGLDebugObject *> *result))completionHandler;

/**
 每一秒调用drawCall次数

 @return 结果
 */
+ (NSUInteger)drawCallsPerSeconds;

@end



@interface EAGLSharegroup (MTGLDebug)


/**
 当前sharegroup内存占用(单位：字节)
 
 @return 当前shareGroup内存占用(单位：字节)
 */
- (size_t)mtgldebug_memorySize;


@end
