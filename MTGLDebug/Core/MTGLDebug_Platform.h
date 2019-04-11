#ifndef MTGLDebug_Platform_h
#define MTGLDebug_Platform_h

// clang-format off

#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
    #ifdef _WIN64
    //define something for Windows (64-bit only)
    #else
    //define something for Windows (32-bit only)
    #endif
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
    // iOS Simulator
        #define MTGLDEBUG_PLATFORM_IOS_SIMULATOR = 1
        #define MTGLDEBUG_PLATFORM_APPLE = 1
    #elif TARGET_OS_IPHONE
    // iOS device
        #define MTGLDEBUG_PLATFORM_IOS_DEVICE = 1
        #define MTGLDEBUG_PLATFORM_APPLE = 1
    #elif TARGET_OS_MAC
    // Other kinds of Mac OS
        #define MTGLDEBUG_PLATFORM_APPLE = 1
    #else
        #error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    // android
    #define MTGLDEBUG_PLATFORM_ANDROID = 1
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
    #error "Unknown compiler"
#endif

// clang-format on

#endif /* MTGLDebug_Platform_h */
