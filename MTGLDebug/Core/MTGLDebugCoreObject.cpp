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


#include "MTGLDebugCoreObject.hpp"
#include "MTGLDebugCore.hpp"
#include "MTGLDebugCoreBridge.hpp"

using namespace GLDebug;

std::string MTGLDebugCoreObject::stringWithType(GLenum type) {
    switch (type) {
        case MT_GLDEBUG_BUFFER:
            return "Buffer";
            break;
        case MT_GLDEBUG_PROGRAM:
            return "Program";
            break;
        case MT_GLDEBUG_TEXTURE:
            return "Texture";
            break;
        case MT_GLDEBUG_FRAMEBUFFER:
            return "Framebuffer";
            break;
        case GL_RENDERBUFFER:
            return "RenderBuffer";
            break;
        default:
            return "未知类型";
            break;
    }
    return nullptr;
}

std::string MTGLDebugCoreObject::debugDescription() {
    std::string type = MTGLDebugCoreObject::stringWithType(target());
    std::string className = typeid(this).name();
    char buffer[100];
    sprintf(buffer, "%p - sharegroup = %p - memorySize = ", *pContext(), *pSharegroup());
    std::string description = "<" + className + "- type =" + type + "- target = " + std::to_string(target()) + "- object = " + std::to_string(object()) + "- context =" + buffer + std::to_string(memorySize()) + "timestamp " + std::to_string(timestampInDouble());
    return description;
}

size_t MTGLDebugCoreObject::calculateMemorySize(GLsizei width, GLsizei height, GLenum format, GLenum type) {
    size_t channels = 0;
    switch (format) {
        case GL_RGBA:
            channels = 4;
            break;
#ifdef MTGLDEBUG_PLATFORM_ANDROID
        case GL_BGRA_EXT:
            channels = 4;
            break;
#else
        case GL_BGRA:
            channels = 4;
            break;
#endif
        case GL_RGB:
            channels = 3;
            break;
        case GL_LUMINANCE:
            channels = 1;
            break;
        case GL_DEPTH_COMPONENT:
            channels = 1;
            break;
        case GL_ALPHA:
            channels = 1;
            break;
        case GL_LUMINANCE_ALPHA:
            channels = 2;
            break;
        case GL_RED_EXT:
            channels = 1;
            break;
        case GL_RG:
            channels = 2;
            break;
        default:
            MTGLDEBUG_printf("format is unsupported!!! \n");
            break;
    }

    float byteLenght = 0;

    switch (type) {
        case GL_UNSIGNED_INT:
            byteLenght = sizeof(GLuint);
            break;
        case GL_UNSIGNED_SHORT:
            byteLenght = sizeof(GLushort);
            break;
        case GL_UNSIGNED_BYTE:
            byteLenght = sizeof(GLubyte);
            break;
        case GL_BYTE:
            byteLenght = sizeof(GLbyte);
            break;
        case GL_SHORT:
            byteLenght = sizeof(GLshort);
            break;
        case GL_INT:
            byteLenght = sizeof(GLint);
            break;
        case GL_FLOAT:
            byteLenght = sizeof(GLfloat);
            break;
        case GL_FIXED:
            byteLenght = sizeof(GLfixed);
            break;
        case GL_UNSIGNED_SHORT_5_6_5:
            byteLenght = sizeof(GLushort) / 3.f;
            break;
        case GL_UNSIGNED_SHORT_5_5_5_1:
            byteLenght = sizeof(GLushort) / 4.f;
            break;
        case GL_UNSIGNED_SHORT_4_4_4_4:
            byteLenght = sizeof(GLushort) / 4.f;
            break;
        default:
            MTGLDEBUG_printf("type is unsupported \n");
            break;
    }

    size_t totalSize = channels * byteLenght * width * height;

    if (!totalSize) {
        if (!MTGLDebugCore::isEnableOnlyStatisticalGLObject()) {
            //            GLDebug::_throw("calculateMemorySize::totalSize is 0");
        }
    }
    return totalSize;
}
