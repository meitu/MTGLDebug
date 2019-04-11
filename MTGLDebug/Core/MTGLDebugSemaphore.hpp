//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/1/11
// Created by: Zed
//


#ifndef MTGLDebugSemaphore_hpp
#define MTGLDebugSemaphore_hpp

#include <condition_variable>
#include <stdio.h>

namespace GLDebug {
class Semaphore
{
  private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long count_ = 0; // Initialized as locked.

  public:
#ifndef MTGLDEBUG_IS_RELEASE
    unsigned long count() {
        return count_;
    }
#endif
    static const unsigned long long SEMAPHORE_TIME_NOW = 0;
    static const unsigned long long SEMAPHORE_TIME_FOREVER = -1;

    Semaphore() = default;
    Semaphore(unsigned long c) {
        count_ = c;
    }

    void notify();

    //equal wait forever
    void wait();

    /*
		 如果 waitfor 返回= 0 说明成功， 不为0说明超时
		 */
    bool waitFor(uint64_t milliSeconds);
};
} // namespace GLDebug

#endif /* MTGLDebugSemaphore_hpp */
