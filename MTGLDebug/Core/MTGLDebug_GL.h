#ifndef MTGLDebug_GL_h
#define MTGLDebug_GL_h

#if __has_include("MTGLDebug_Platform.h")
#include "MTGLDebug_Platform.h"
#else
#ifdef COCOAPODS
#include "MTGLDebug_Platform.h"
#else
#error "MTGLDebug_Platform.h not found!"
#endif
#endif

#if defined(MTGLDEBUG_PLATFORM_IOS_DEVICE) || defined(MTGLDEBUG_PLATFORM_IOS_SIMULATOR)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/gl.h>
#elif defined(MTGLDEBUG_PLATFORM_ANDROID)
#include <GLES3/gl3.h>
#define __gl2_h_
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#endif

#endif /* MTGLDebug_GL_h */
