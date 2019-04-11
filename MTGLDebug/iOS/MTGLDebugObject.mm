//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/7/12
// Created by: wuzeqin
//


#import "MTGLDebugObject.h"
#include "MTGLDebugCore.hpp"
#import "MTGLDebugObject_Internal.h"

using namespace GLDebug;

@implementation MTGLDebugObject

+ (instancetype)covertCplusplusObject:(MTGLDebugCoreObject *)obj {

    if (!obj) {
        return nil;
    }

    Class objClass;

    switch (obj->classType()) {
        case MTGLDebugFramebuffeCoreObjectClass:
        case MTGLDebugCoreObjectBaseClass:
            objClass = [MTGLDebugObject class];
            break;
        case MTGLDebugCoreObjectTextureClass:
            objClass = [MTGLDebugTextureObject class];

            break;
        case MTGLDebugCoreObjectCVTextureClass:
            objClass = [MTGLDebugCVObject class];
            break;
        case MTGLDebugProgramCoreObjectClass:
            objClass = [MTGLDebugProgramObject class];
            break;

        default:
            break;
    }
    return [[objClass alloc] initWithCplusplusObject:obj];
}

- (instancetype)initWithCplusplusObject:(MTGLDebugCoreObject *)obj {
    self = [super init];
    if (self) {
        _target = obj->target();
        _object = obj->object();
        const void **ctx = obj->pContext();
        _context = *ctx ? (__bridge EAGLContext *)*ctx : nil;
        _memorySize = obj->memorySize();
        const void **sgp = obj->pSharegroup();
        _sharegroup = *sgp ? (__bridge EAGLSharegroup *)*sgp : nil;
        _timestampInDouble = obj->timestampInDouble();
        _timestamp = [self currentTimestamp:_timestampInDouble];
    }
    return self;
}

- (NSString *)currentTimestamp:(double)timestamp {
    static NSDateFormatter *dateFormatter;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"CCD"]];
        [dateFormatter setDateFormat:@"HH:mm:ss:SSS"];
    });
    return [dateFormatter stringFromDate:[NSDate dateWithTimeIntervalSince1970:timestamp]];
}

- (NSString *)targetString {
    return [NSString stringWithUTF8String:GLDebug::MTGLDebugCoreObject::stringWithType(self.target).c_str()];
}
@end

@implementation MTGLDebugTextureObject
- (instancetype)initWithCplusplusObject:(MTGLDebugTextureCoreObject *)obj {
    self = [super initWithCplusplusObject:obj];
    if (self) {
        _width = obj->width();
        _height = obj->height();
    }
    return self;
}



@end

@implementation MTGLDebugCVObject

- (instancetype)initWithCplusplusObject:(MTGLDebugCVCoreObject *)obj {
    self = [super initWithCplusplusObject:obj];
    if (self) {
        _child = [MTGLDebugObject covertCplusplusObject:obj->child()];
        _isFromCamera = obj->isFromCamera;
    }
    return self;
}
@end

@implementation MTGLDebugProgramObject

- (instancetype)initWithCplusplusObject:(MTGLDebugProgramCoreObject *)obj {
    self = [super initWithCplusplusObject:obj];
    if (self) {
        _vertexShader = [NSString stringWithUTF8String:obj->vertexShader().c_str()];
        _fragmentShader = [NSString stringWithUTF8String:obj->fragmentShader().c_str()];
    }
    return self;
}

@end
