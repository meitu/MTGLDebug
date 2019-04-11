//
// Copyright (c) 2008-present, Meitu, Inc.
// All rights reserved.
//
// This source code is licensed under the license found in the LICENSE file in
// the root directory of this source tree.
//
// Created on: 2017/12/29
// Created by: Zed
//


#include "MTGLDebugCore.hpp"

namespace GLDebug {
class MTGLDebugCore_iOS : public MTGLDebugCore
{

  protected:
    bool deleteObject(const MTGLDebugCoreInputObject &inputObject);

    MTGLDebugCoreGLAPIVersion currentGLAPIVersion(void);
};
} // namespace GLDebug
