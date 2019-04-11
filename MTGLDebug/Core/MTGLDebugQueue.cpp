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


#include "MTGLDebugQueue.hpp"
#include "MTGLDebug_Macro.h"
#include "MTGLDebug_Platform.h"

#ifdef MTGLDEBUG_PLATFORM_APPLE
#include "MTGLDebugQueue_iOS_Bridge.h"
#endif

using namespace GLDebug;

Queue::~Queue() {
    mStop = true;
    mWaitLock->GetAnotherChance();
    mRunThread->join();
    delete mRunThread;


    if (mOperationCache) {
        Operation *operation = NULL;
        while (mOperationCache->size()) {
            operation = mOperationCache->front();
            delete operation;
            operation = NULL;
            mOperationCache->pop();
        }
        delete mOperationCache;
    }

    delete mOperations;
    delete mWaitLock;
    MTGLDEBUG_printf("GLDebug::Queue dealloc \n");
}

void Queue::cancelAllOperations() {
    this->runOperationInResourceLock([=] {
        while (mOperations->size()) {
            mOperationCache->push(mOperations->front());
            mOperations->pop();
        }
    });
}

void Queue::runOperationInResourceLock(const std::function<void()> &operation) {
    mResourceLock.lock();
    operation();
    mResourceLock.unlock();
}

Queue::Queue() {
    mWaitLock = new ConditionLock([&]() -> bool {
        return this->mOperations->size() > 0 || mStop;
    });
    mOperationCache = new std::queue<Operation *>();
    mOperations = new std::queue<Operation *>();
    mRunThread = new std::thread(&Queue::runLoop, this);
}

void Queue::setName(const std::string &name) {
    addSync([=] {
#ifdef MTGLDEBUG_PLATFORM_ANDROID
        std::string newName = name;

        if (name.length() > 16) {
            newName = name.substr(0, 15);
        }

        int result = pthread_setname_np(pthread_self(), newName.c_str());
        MTGLDEBUG_assert(!result, "set thread is failed");
#else
        pthread_setname_np(name.c_str());
#endif
    });
}


void Queue::addSync(const std::function<void()> &operation) {
    this->addOperation(operation, false);
}

void Queue::addAsync(const std::function<void()> &operation) {
    this->addOperation(operation, true);
}

void Queue::addOperation(const std::function<void()> &operation, bool async) {

    if (mStop) {
        return;
    }

    Operation *tmpOperation = NULL;
    this->runOperationInResourceLock([&] {
        if (mOperationCache->size()) {
            tmpOperation = mOperationCache->front();
            tmpOperation->reset(operation, async);
            mOperationCache->pop();
        } else {
            tmpOperation = new Operation(operation, async);
        }

        mOperations->push(tmpOperation);
        mWaitLock->GetAnotherChance();
    });

    if (!async) {
        tmpOperation->forceWaitToFinished();
        this->runOperationInResourceLock([=] {
            mOperationCache->push(tmpOperation);
        });
    }
}

void Queue::waitUntilAllOperationsAreFinished() {
    this->addSync(NULL);
}

void Queue::runLoop() {
    while (!mStop) {
        Operation *operation = NULL;

        this->runOperationInResourceLock([&] {
            if (mOperations->size()) {
                operation = mOperations->front();
                mOperations->pop();
            }
        });


        if (operation) {
            bool isAsync = operation->isAsync();
            MTGLDEBUG_APPLE(void *autoreleasePool = NULL; mtGLDebugAutoreleasePoolPush(&autoreleasePool);)
            operation->run();
            MTGLDEBUG_APPLE(mtGLDebugautoreleasePoolPop(autoreleasePool);)
            operation->done();
            this->runOperationInResourceLock([=] {
                if (isAsync) {
                    mOperationCache->push(operation);
                }
            });
        }

        mWaitLock->Verification();
    }


    this->runOperationInResourceLock([=] {
        Operation *operation = NULL;
        while (mOperations->size()) {
            operation = mOperations->front();
            if (!operation->isAsync()) {
                operation->done();
            }
            delete operation;
            operation = NULL;
            mOperations->pop();
        }
    });
}
