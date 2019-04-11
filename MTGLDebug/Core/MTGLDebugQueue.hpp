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


#ifndef MTGLDebugQueue_hpp
#define MTGLDebugQueue_hpp

#include <assert.h>
#include <queue>
#include <stdio.h>
#include <string>
#include <thread>
#include "MTGLDebugConditionLock.hpp"
#include "MTGLDebugSemaphore.hpp"


namespace GLDebug {
class Queue;
class Operation
{
    std::function<void()> mOperation = NULL;
    ConditionLock *mWaitLock = NULL;
    bool mAsync = false;
    bool mFinished = false;

    friend class Queue;

  private:
    void run() {
        assert(!mFinished);
        if (mOperation) {
            mOperation();
        }
    }

    void done() {
        mFinished = true;
        mOperation = nullptr;
        mWaitLock->GetAnotherChance();
    }

    void forceWaitToFinished() {
        mWaitLock->Verification();
    }

    void reset(const std::function<void()> &operation, bool async) {
        mOperation = operation;
        mAsync = async;
        mFinished = false;
    }

  public:
    ~Operation() {
        delete mWaitLock;
    }

    Operation(const std::function<void()> &operation, bool async) {
        reset(operation, async);
        mWaitLock = new ConditionLock([&]() -> bool {
            return mFinished;
        });
    }

    bool isAsync() {
        return mAsync;
    }
};

class Queue
{
  private:
    std::queue<Operation *> *mOperations = NULL;
    std::queue<Operation *> *mOperationCache = NULL;
    std::thread *mRunThread = NULL;
    ConditionLock *mWaitLock = NULL;
    std::mutex mResourceLock;
    bool mStop = false;
    void runLoop();
    void runOperationInResourceLock(const std::function<void()> &operation);

  public:
    Queue();
    ~Queue();
    Queue(const Queue &queue) = delete;
    void setName(const std::string &name);
    void addAsync(const std::function<void()> &operation);
    void addSync(const std::function<void()> &operation);
    void addOperation(const std::function<void()> &operation, bool async);
    void waitUntilAllOperationsAreFinished();
    void cancelAllOperations();
    bool isCurrentQueue() {
        auto threadID = mRunThread->get_id();
        return std::this_thread::get_id() == threadID;
    };
};
} // namespace GLDebug

#endif /* MTGLDebugQueue_hpp */
