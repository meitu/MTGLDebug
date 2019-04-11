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


#ifndef MTGLDebugCoreObject_hpp
#define MTGLDebugCoreObject_hpp

#include <chrono>
#include <map>
#include <stdio.h>
#include <string>
#include "MTGLDebug_GL.h"

namespace GLDebug {

enum MTGLDebugCoreObjectClass {
    //MTGLDebugcoreObject
    MTGLDebugCoreObjectBaseClass = 1 << 0,

    //MTGLDebugTextureCoreObject
    MTGLDebugCoreObjectTextureClass = 1 << 1,

#ifdef MTGLDEBUG_PLATFORM_APPLE
    //MTGLDebugCVCoreObject
    MTGLDebugCoreObjectCVTextureClass = 1 << 2,
#endif
    //MTGLDebugProgramCoreObject
    MTGLDebugProgramCoreObjectClass = 1 << 3,

    //MTGLDebugFramebufferObject
    MTGLDebugFramebuffeCoreObjectClass = 1 << 4
};



class MTGLDebugCoreObject
{

  public:
#ifdef MTGLDEBUG_PLATFORM_ANDROID
    void retain() {
        mReferenceCount++;
    }

    void release() {
        mReferenceCount--;
    }

    unsigned int referenceCount() const {
        return mReferenceCount;
    }
#endif
    int classType() const {
        return mClassType;
    }

    GLenum target() const {
        return mTarget;
    }

    GLuint object() const {
        return mObject;
    }

    const void **pContext() const {
        return m_pContext;
    }

    size_t memorySize() const {
        return mMemorySize;
    }

    double timestampInDouble() const {
        return mTimestampInDouble;
    }

    const void **pSharegroup() const {
        return m_pSharegroup;
    }

    static size_t calculateMemorySize(GLsizei width,
        GLsizei height,
        GLenum format,
        GLenum type);

    static std::string stringWithType(GLenum type);

    std::string objectID() {
        return std::to_string(mTarget) + "-" + std::to_string(mObject);
    }

    bool operator>(const MTGLDebugCoreObject &obj) const {
        return memorySize() > obj.memorySize();
    }

    bool operator==(const MTGLDebugCoreObject &obj) const {
        bool result = target() == obj.target() && object() == obj.object() && (*pContext() == *obj.pContext() || *pSharegroup() == *obj.pSharegroup());
        return result;
    }

    bool operator!=(const MTGLDebugCoreObject &obj) const {
        return !(*this == obj);
    }

    MTGLDebugCoreObject &operator=(const MTGLDebugCoreObject &obj) {
        this->mClassType = obj.classType();
        this->mTarget = obj.target();
        this->mObject = obj.object();


        this->m_pContext = obj.pContext();
        this->m_pSharegroup = obj.pSharegroup();

        this->mMemorySize = obj.memorySize();
        this->mTimestampInDouble = obj.timestampInDouble();
#ifdef MTGLDEBUG_PLATFORM_ANDROID
        this->mReferenceCount = obj.referenceCount();
#endif
        return *this;
    }

    virtual std::string debugDescription();

    void SetMemorySize(size_t size) {
        mMemorySize = size;
    }

    void appendSize(size_t size) {
        mMemorySize += size;
    }

    MTGLDebugCoreObject()
        : mClassType(MTGLDebugCoreObjectBaseClass){};

    MTGLDebugCoreObject(GLenum target,
        GLuint object,
        const void *context,
        const void *sharegroup,
        size_t memorySize) {
        mClassType = MTGLDebugCoreObjectBaseClass;
        mTarget = target;
        mObject = object;

        m_pContext = &context;
        m_pSharegroup = &sharegroup;

        mMemorySize = memorySize;
        mTimestampInDouble = std::chrono::system_clock::now().time_since_epoch().count() / 1000000.0;
    }

    MTGLDebugCoreObject(const MTGLDebugCoreObject &obj) {
        mClassType = obj.classType();
        mTarget = obj.target();
        mObject = obj.object();


        m_pContext = obj.pContext();


        m_pSharegroup = obj.pSharegroup();

        mMemorySize = obj.memorySize();
        mTimestampInDouble = obj.timestampInDouble();
#ifdef MTGLDEBUG_PLATFORM_ANDROID
        mReferenceCount = obj.referenceCount();
#endif
    }

    virtual ~MTGLDebugCoreObject() {
        mClassType = 0;
        mTarget = 0;
        mObject = 0;

        m_pSharegroup = 0;
        m_pContext = 0;

        mMemorySize = 0;
        mTimestampInDouble = 0;
    };

  protected:
#ifdef MTGLDEBUG_PLATFORM_ANDROID
    unsigned int mReferenceCount = 1;
#endif
    int mClassType = MTGLDebugCoreObjectBaseClass;
    GLenum mTarget = 0;
    GLuint mObject = 0;
    const void **m_pContext = NULL; //为了防止EAGLContext被释放导致野指针这边采用指针的指针存储
    size_t mMemorySize = 0;
    const void **m_pSharegroup = NULL; //为了防止EAGLSharegroup被释放导致野指针这边采用指针的指针存储
    double mTimestampInDouble = 0;
};

class MTGLDebugTextureCoreObject : public MTGLDebugCoreObject
{

  public:
    MTGLDebugTextureCoreObject &operator=(const MTGLDebugTextureCoreObject &obj) {
        MTGLDebugCoreObject::operator=(obj);
        mWidth = obj.width();
        mHeight = obj.height();
        return *this;
    }


    MTGLDebugTextureCoreObject()
        : MTGLDebugCoreObject(){};

    MTGLDebugTextureCoreObject(GLenum target,
        GLuint object,
        const void *context,
        const void *sharegroup,
        size_t memorySize)
        : MTGLDebugCoreObject(target, object, context, sharegroup, memorySize) {
        mClassType = MTGLDebugCoreObjectTextureClass;
    }

    MTGLDebugTextureCoreObject(const MTGLDebugTextureCoreObject &obj)
        : MTGLDebugCoreObject(obj) {
        mWidth = obj.width();
        mHeight = obj.height();
    }

    GLsizei width() const {
        return mWidth;
    }

    GLsizei height() const {
        return mHeight;
    }

    void setWidth(GLsizei width) {
        mWidth = width;
    }

    void setHeight(GLsizei height) {
        mHeight = height;
    }
    virtual ~MTGLDebugTextureCoreObject() {
        mWidth = 0;
        mHeight = 0;
    };

  protected:
    GLsizei mWidth = 0;
    GLsizei mHeight = 0;
};

typedef MTGLDebugTextureCoreObject MTGLDebugRenderbufferCoreObject;


class MTGLDebugProgramCoreObject : public MTGLDebugCoreObject
{

  public:
    MTGLDebugProgramCoreObject &operator=(const MTGLDebugProgramCoreObject &obj) {
        MTGLDebugCoreObject::operator=(obj);
        mVertexShaderString = obj.vertexShader();
        mFragmentShaderString = obj.fragmentShader();
        return *this;
    }
    MTGLDebugProgramCoreObject(const MTGLDebugProgramCoreObject &obj)
        : MTGLDebugCoreObject(obj) {
        mVertexShaderString = obj.vertexShader();
        mFragmentShaderString = obj.fragmentShader();
    }

    MTGLDebugProgramCoreObject(GLenum target,
        GLuint object,
        const void *context,
        const void *sharegroup,
        size_t memorySize)
        : MTGLDebugCoreObject(target, object, context, sharegroup, memorySize) {
        mClassType = MTGLDebugProgramCoreObjectClass;
    }


    std::string vertexShader() const {
        return mVertexShaderString;
    }

    std::string fragmentShader() const {
        return mFragmentShaderString;
    }

    void setVertexShader(std::string vStr) {
        mVertexShaderString = vStr;
    }

    void setFragmentShader(std::string fStr) {
        mFragmentShaderString = fStr;
    }
    virtual ~MTGLDebugProgramCoreObject() {
        mVertexShaderString.clear();
        mFragmentShaderString.clear();
    };

  protected:
    std::string mVertexShaderString;
    std::string mFragmentShaderString;
};

class MTGLDebugFramebuffeCoreObject : public MTGLDebugCoreObject
{

  public:
    MTGLDebugFramebuffeCoreObject(GLenum target,
        GLuint object,
        const void *context,
        const void *sharegroup,
        size_t memorySize)
        : MTGLDebugCoreObject(target, object, context, sharegroup, memorySize) {
        mClassType = MTGLDebugFramebuffeCoreObjectClass;
    }

    void setTexture(unsigned int id) {
        mBindingTexture = id;
        mIsBindingTexture = true;
    }

    unsigned int getTexture() {
        return mBindingTexture;
    }

    bool isBindingTexture() {
        return mIsBindingTexture;
    }

    void setIsBindingTexture(bool value) {
        mIsBindingTexture = value;
    }

    virtual ~MTGLDebugFramebuffeCoreObject() {
        mBindingTexture = 0;
        mIsBindingTexture = false;
    }

  protected:
    unsigned int mBindingTexture = 0;
    bool mIsBindingTexture = false;
};



#ifdef MTGLDEBUG_PLATFORM_APPLE

class MTGLDebugCVCoreObject : public MTGLDebugTextureCoreObject
{

  private:
    MTGLDebugTextureCoreObject *mChild = NULL;

    void copyChild(MTGLDebugTextureCoreObject *child) {
        mChild = new MTGLDebugTextureCoreObject(*(MTGLDebugTextureCoreObject *)child);
    }

    void removeChild() {
        delete mChild;
        mChild = NULL;
    }

  public:
    MTGLDebugCVCoreObject()
        : MTGLDebugTextureCoreObject(){};

    MTGLDebugCVCoreObject(GLenum target,
        GLuint object,
        const void *context,
        const void *sharegroup,
        size_t memorySize)
        : MTGLDebugTextureCoreObject(target, object, context, sharegroup, memorySize) {
        mClassType = MTGLDebugCoreObjectCVTextureClass;
    }

    MTGLDebugCVCoreObject(const MTGLDebugCVCoreObject &obj)
        : MTGLDebugTextureCoreObject(obj) {
        isFromCamera = obj.isFromCamera;
        copyChild(obj.mChild);
    }

    MTGLDebugCVCoreObject &operator=(const MTGLDebugCVCoreObject &obj) {
        MTGLDebugTextureCoreObject::operator=(obj);
        isFromCamera = obj.isFromCamera;
        copyChild(obj.mChild);
        return *this;
    }



    MTGLDebugTextureCoreObject *child() {
        return mChild;
    }

    void setChild(MTGLDebugTextureCoreObject *nChild) {
        mChild = nChild;
    }


    /**
         是否是系统数据流产生的纹理
         */
    bool isFromCamera;

    ~MTGLDebugCVCoreObject() {
        removeChild();
    }
};
#endif

} // namespace GLDebug



#endif /* MTGLDebugCoreObject_hpp */
