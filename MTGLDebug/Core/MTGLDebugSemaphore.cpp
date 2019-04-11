//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2018/4/16
// Created by: Zed
//


#include "MTGLDebugSemaphore.hpp"
#include "MTGLDebug_Macro.h"


using namespace GLDebug;

void Semaphore::notify() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    ++count_;
    condition_.notify_one();
}

void Semaphore::wait() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    while (!count_) // Handle spurious wake-ups.
    {
        condition_.wait(lock);
    }
    --count_;
}

bool Semaphore::waitFor(uint64_t milliSeconds) {
    auto ms = std::chrono::milliseconds(1);
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    while (!count_) {
        std::cv_status status = condition_.wait_for(lock, milliSeconds * ms);
        if (status == std::cv_status::no_timeout) {
            --count_;
            return false;
        } else {
            return true;
        }
    }
    --count_;
    return false;
}
