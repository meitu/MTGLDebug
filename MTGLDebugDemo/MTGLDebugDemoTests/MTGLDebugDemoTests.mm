//
//  MTGLDebugTests.m
//  MTGLDebugTests
//
//  Created by Zed on 2017/3/27.
//  Copyright © 2017年 meitu. All rights reserved.
//

#import <XCTest/XCTest.h>

#import <MTGLDebug/MTGLDebugCore.hpp>
#import <MTGLDebug/MTGLDebugCoreBridge.hpp>
#import <MTGLDebug/MTGLDebugImpl_Internal.h>

#define MT_GL_TEXTURE 0x4

@interface MTGLDebug (UnitTest)


+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
                    errorType:(unsigned int *)type;

+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
                    initClass:(int)initClass
                    errorType:(unsigned int *)type;

+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
      outputShareGroupObjects:(void *)outputShareGroupObjects
                    initClass:(int)initClass
                    errorType:(unsigned int *)type;

+ (BOOL)mtgldebug_checkObject:(GLuint)object target:(GLenum)target;



@end

@implementation MTGLDebug (UnitTest)

+ (BOOL)mtgldebug_isCanDeleteObject:(GLuint)object target:(GLenum)target initClass:(int)initClass {
    return [MTGLDebug mtgldebug_checkObject:object target:target initClass:initClass errorType:nil];
}

+ (BOOL)mtgldebug_checkObject:(GLuint)object target:(GLenum)target {
    return [MTGLDebug mtgldebug_checkObject:object target:target errorType:nil];
}

+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
                    errorType:(unsigned int *)type {
    return MTGLDebugCore_CheckObject(object,
        target,
        NULL,
        1 << 1, /** 0:base, 1:texture,iOS:2:CVObject**/
        type);
}

+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
                    initClass:(int)initClass
                    errorType:(unsigned int *)type {
    return [MTGLDebug mtgldebug_checkObject:object
                                     target:target
                    outputShareGroupObjects:NULL
                                  initClass:initClass
                                  errorType:type];
}

+ (BOOL)mtgldebug_checkObject:(GLuint)object
                       target:(GLenum)target
      outputShareGroupObjects:(void *)outputShareGroupObjects
                    initClass:(int)initClass
                    errorType:(unsigned int *)type {
    return MTGLDebugCore_CheckObject(object,
        target,
        outputShareGroupObjects,
        initClass, /** 0:base, 1:texture,iOS:2:CVObject**/
        type);
}

@end


@interface MTGLDebugUnitTestTests : XCTestCase

@end

@implementation MTGLDebugUnitTestTests

+ (void)load {
    [MTGLDebug registerMTGLDebugHook];
    [MTGLDebug setEnableGLDebugException:NO];
}
// 正常使用
- (void)testCase1 {
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint texture;
    glGenTextures(1, &texture);
    glDeleteTextures(1, &texture);

    XCTAssertFalse([MTGLDebug mtgldebug_isCanDeleteObject:texture
                                                   target:MT_GL_TEXTURE
                                                initClass:1 << 1],
        @"删除纹理失败");
    NSLog(@"testCase1:正常使用，测试通过");
}

// 一个上下文创建，另一个上下文使用或者销毁
- (void)testCase2 {
    EAGLContext *contextA = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:contextA];
    GLuint texture;
    glGenTextures(1, &texture);

    EAGLContext *contextB = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:contextB];


    XCTAssertFalse([MTGLDebug mtgldebug_isCanDeleteObject:texture
                                                   target:MT_GL_TEXTURE
                                                initClass:1 << 1],
        @"当前上下文删除纹理失败");

    NSLog(@"testCase2:检测出非当前上下文所创建的对象，测试通过");
    [EAGLContext setCurrentContext:contextA];
    glDeleteTextures(1, &texture);
}

// 创建上下文A，使用A的sharegroup创建上下文B，B使用A创建出来的对象。
- (void)testCase3 {
    EAGLContext *contextA = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:contextA];
    GLuint texture;
    glGenTextures(1, &texture);

    EAGLContext *contextB = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2
                                                 sharegroup:contextA.sharegroup];
    [EAGLContext setCurrentContext:contextB];

    XCTAssertTrue([MTGLDebug mtgldebug_checkObject:texture
                                            target:MT_GL_TEXTURE],
        @"当前上下文检查纹理失败");
    NSLog(@"testCase3:检测出同一个sharegroup所创建的对象可以使用，测试通过");
    glBindTexture(GL_TEXTURE_2D, texture);

    XCTAssertTrue([MTGLDebug mtgldebug_isCanDeleteObject:texture
                                                  target:MT_GL_TEXTURE
                                               initClass:1 << 1],
        @"当前上下文删除纹理失败");

    NSLog(@"testCase3:检测出同一个sharegroup所创建的对象可以删除，测试通过");
    glDeleteTextures(1, &texture);
}



//  Created by Zed.

//当前上下文不包含这个对象!!!!!!,这个GL对象不存在未被创建过
- (void)testCase5 {
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint texture = 9999;
    unsigned int type;
    //下面的操作相当于 glBindTexture(0x99999996, texture)
    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:texture
                                             target:MT_GL_TEXTURE
                                          errorType:&type],
        @"testCase5:纹理绑定成功,测试不通过");
    XCTAssertTrue(type == 0, @"testCase5:返回错误类型出错,测试不通过");
    NSLog(@"testCase5:纹理绑定失败,这个GL对象不存在未被创建过,测试通过");
}

//当前上下文不包含这个对象!!!!!!,在当前上下文创建过但是被销毁,绑定错上下文导致误删
- (void)testCase6 {
    GLuint texture, tempTexture;
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    glGenTextures(1, &tempTexture);
    EAGLContext *context1 = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context1];
    glGenTextures(1, &texture);
    glDeleteTextures(1, &tempTexture);
    unsigned int type1;
    NSLog(@"%p",&type1);
    //下面的操作相当于 glBindTexture(0x99999996, texture)
    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:texture
                                             target:MT_GL_TEXTURE
                                          initClass:1 << 0 //[MTGLDebugTextureObject class]
                                          errorType:&type1],
        @"testCase6:纹理绑定成功,测试不通过");

    XCTAssertTrue(type1 == 2, @"testCase6:返回错误类型出错,测试不通过");

    NSLog(@"testCase6:纹理绑定失败,在当前上下文创建过但是被销毁,绑定错上下文导致误删,测试通过");

    //操作结束删除临时context创建的gl对象
    [EAGLContext setCurrentContext:context];
    glDeleteTextures(1, &tempTexture);
}

//当前上下文不包含这个对象!!!!!!,在当前上下文创建过但是被销毁,使用已被删除的对象
- (void)testCase7 {
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint texture7;
    glGenTextures(1, &texture7);
    glDeleteTextures(1, &texture7);
    unsigned int type;
    //下面的操作相当于 glBindTexture(0x99999996, texture7)
    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:texture7
                                             target:MT_GL_TEXTURE
                                          initClass:1 << 1 //(void *)[MTGLDebugTextureObject class]
                                          errorType:&type],
        @"testCase7:纹理绑定成功,测试不通过");
    XCTAssertTrue(type == 1, @"testCase7:返回错误类型出错,测试不通过");
    NSLog(@"testCase7:纹理绑定失败,在当前上下文创建过但是被销毁,使用已被删除的对象,测试通过");
}

//当前上下文不包含这个对象!!!!!!,在其他上下文操作,绑定错上下文导致误删

- (void)testCase8 {
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    // 像这样声明纹理对象 texture7[0]和 texture8是同一个内存区域的纹理。 这样的话岂不是glGen只生成一个纹理都是同一个，或者要绑定之后纹理ID才能增加？ 整个纹理ID是怎样工作的？
    /*
     GLuint *texture7;
     glGenTextures(1, texture7);
     [MTGLController bind];
     GLuint texture8;
     glGenTextures(1, &texture8);
     NSUInteger type;
     //下面的操作相当于 glBindTexture(0x99999996, texture7)
     if ([MTGLDebug mtgldebug_checkObject:texture7
     target:0x99999996
     initClass:[MTGLDebugTextureObject class]
     errorType:&type]) {
     XCTAssertFalse(@"testCase8:纹理绑定成功,测试不通过");
     }else {
     if (type == 3) {
     NSLog(@"testCase8:纹理绑定失败,在当前上下文创建过但是被销毁,使用已被删除的对象,测试通过");
     }else {
     NSLog(@"%lu",(unsigned long)type);
     XCTAssertFalse(@"testCase8:返回错误类型出错,测试不通过");
     }
     }
     //操作结束删除临时context创建的gl对象
     [EAGLContext setCurrentContext:context];
     glDeleteTextures(1, texture7);
     */


    GLuint *texture7 = (GLuint *)malloc(sizeof(GLuint) * 2);
    glGenTextures(2, texture7);

    EAGLContext *context1 = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context1];
    GLuint texture8;
    glGenTextures(1, &texture8);
    unsigned int type;
    //下面的操作相当于 glBindTexture(0x99999996, texture7)
    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:texture7[1]
                                             target:MT_GL_TEXTURE
                                          initClass:1 << 1 //(void *)[MTGLDebugTextureObject class]
                                          errorType:&type],
        @"testCase8:纹理绑定成功,测试不通过");
    XCTAssertTrue(type == 3, @"testCase8:返回错误类型出错,测试不通过");

    NSLog(@"testCase8:纹理绑定失败,在当前上下文创建过但是被销毁,使用已被删除的对象,测试通过");

    //操作结束删除临时context创建的gl对象
    [EAGLContext setCurrentContext:context];
    glDeleteTextures(2, texture7);
    free(texture7);

    [EAGLContext setCurrentContext:context1];
    glDeleteTextures(1, &texture8);
}

- (void)testCase9 {
    /**
	 测试 仅统计GL对象内存开关是否生效
	 生成一个纹理他内存为 sizeof(GLuint) 4K
	 查看统计内存是否正确
	 删除该纹理
	 chack应该不生效
	 */
    [MTGLDebug setEnableOnlyStatisticalGLObject:YES];
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint texture9;
    glGenTextures(1, &texture9);
    glBindTexture(GL_TEXTURE_2D, texture9);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    NSArray *a = [MTGLDebug sortedDebugObjectsByMemory];
    MTGLDebugObject *obj = a[0];

    XCTAssertTrue((obj.memorySize == (500 * 200 * 4 + 4)), @"testCase9:统计的纹理错误,测试不通过");

    glDeleteTextures(1, &texture9);
    GLDebug::MTGLDebugCoreObjectMap *shareGroupObjects = NULL;

    XCTAssertTrue([MTGLDebug mtgldebug_checkObject:texture9
                                            target:MT_GL_TEXTURE
                           outputShareGroupObjects:&shareGroupObjects
                                         initClass:1 << 1 //(void *)[MTGLDebugTextureObject class]
                                         errorType:NULL],
        @"testCase9:返回false,测试不通过");
    NSLog(@"使用已删除的texture,原本应该返回 false 但是由于仅开启内存检测,所以返回 true");


    [MTGLDebug setEnableOnlyStatisticalGLObject:NO];
    [EAGLContext setCurrentContext:context];
    GLuint texture19;
    glGenTextures(1, &texture19);
    glBindTexture(GL_TEXTURE_2D, texture19);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    NSArray *a1 = [MTGLDebug sortedDebugObjectsByMemory];
    MTGLDebugObject *obj1 = a1[0];


    XCTAssertTrue(obj1.memorySize == (100 * 200 * 4 + 4), @"testCase9:统计的纹理错误,测试不通过");

    glDeleteTextures(1, &texture19);
    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:texture19
                                             target:MT_GL_TEXTURE
                            outputShareGroupObjects:&shareGroupObjects
                                          initClass:1 << 1 //(void *)[MTGLDebugTextureObject class]
                                          errorType:NULL],
        @"testCase9:纹理绑定成功,测试不通过");
    NSLog(@"使用已删除的texture,应该返回 false");
}

- (void)testCase10 {
}


- (void)testCase11 {

    GLubyte *_maskTextureData;
    CGSize maskImageSize = CGSizeMake(1000, 1000);
    _maskTextureData = (GLubyte *)calloc(maskImageSize.width * maskImageSize.height, sizeof(GLubyte));

    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
   
    GLuint *textureID = (GLuint *)malloc(sizeof(GLuint));
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLint unPackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unPackAlignment);
  
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, maskImageSize.width, maskImageSize.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _maskTextureData);
    
    CGRect rect = CGRectMake(0, 0, maskImageSize.width, maskImageSize.height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    rect.origin.x,
                    rect.origin.y,
                    rect.size.width,
                    rect.size.height,
                    GL_ALPHA,
                    GL_UNSIGNED_BYTE,
                    _maskTextureData);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unPackAlignment);
    


    XCTAssertFalse([MTGLDebug mtgldebug_checkObject:*textureID
                                             target:MT_GL_TEXTURE],
        @"testCase11:数据不对齐,当前object会被删除,应该检测不到这个maskTexture,所以测试不通过");


    NSLog(@"testCase11:因为当前texture数据格式不对齐,所以删除当前纹理,检测不到这个maskTexture,测试通过");

    free(_maskTextureData);
    
    if (textureID && *textureID) {
        glDeleteTextures(1, textureID);
        *textureID = 0;
        free(textureID);
    }
    
    _maskTextureData = NULL;
}

- (void)testCase12 {
    //errorParameterInvokeGLAPI
    //错误的GLClear调用，但是因为开启只检测内存，不会异常抛出，并且内存为4，测试通过
    [MTGLDebug setEnableOnlyStatisticalGLObject:YES];
    [MTGLDebug setEnableGLDebugException:YES];

    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //验证gl操作错误异常抛出开关
    NSArray *a1 = [MTGLDebug sortedDebugObjectsByMemory];
    MTGLDebugObject *obj1 = a1[0];

    XCTAssertTrue((obj1.memorySize == 4), @"testCase12:统计的纹理错误,测试不通过");

    //测试完毕释放，关闭仅内存检测开关
    glDeleteFramebuffers(1, &fbo);
    [MTGLDebug setEnableOnlyStatisticalGLObject:NO];
    [MTGLDebug setEnableGLDebugException:NO];
}

- (void)testCase14 {
    //测试用例是否没有释放完毕
    size_t memorySize = [MTGLDebug fetchOpenGLESResourceMemorySize];
    NSArray<MTGLDebugObject *> *objects = [MTGLDebug sortedDebugObjectsByMemory];

    XCTAssertTrue((memorySize == 0 && objects.count == 0), @"内存有残留,测试不通过");

    NSLog(@"testCase4:内存没有残留，测试通过");
}


// 创建Program
- (void)testCase16 {
}

// 创建一个fbo绑定texture删除texture后再次bindfbo
- (void)testCase17 {
    EAGLContext *context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    [EAGLContext setCurrentContext:context];
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glDeleteTextures(1, &texture);

    XCTAssertFalse(MTGLDebugCore_CheckObject(fbo, MT_GLDEBUG_FRAMEBUFFER, NULL, MTGLDebugCoreObjectFramebuffer, NULL));
    glDeleteFramebuffers(1, &fbo);
}


@end
