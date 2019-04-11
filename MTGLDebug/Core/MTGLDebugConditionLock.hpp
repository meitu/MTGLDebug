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


#ifndef MTGLDebugConditionLock_hpp
#define MTGLDebugConditionLock_hpp

#include <assert.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdio.h>

namespace GLDebug {
class ConditionLock
{
    std::mutex mLock;
    std::condition_variable mConditionvariable;
    std::function<bool()> mConditaion;

  public:
    ConditionLock(const std::function<bool()> &condition) {
        mConditaion = condition;
    }

    void Verification() {
        assert(mConditaion);
        std::unique_lock<decltype(mLock)> lock(mLock);
        while (!mConditaion())
            mConditionvariable.wait(lock);
    };

    void GetAnotherChance() {
        std::unique_lock<decltype(mLock)> lock(mLock);
        mConditionvariable.notify_one();
    }
};
} // namespace GLDebug

#endif /* MTGLDebugConditionLock_hpp */
