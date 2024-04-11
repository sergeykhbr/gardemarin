/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include "CommonInterface.h"
#include <canframe.h>

class CanListenerInterface : public CommonInterface {
 public:
    CanListenerInterface() : CommonInterface("CanListenerInterface") {}

    virtual void CanCallback(can_frame_type *frame) = 0;
};


class CanInterface : public CommonInterface {
 public:
    CanInterface() : CommonInterface("CanInterface") {}

    virtual void RxInterruptHandler(int fifoid) = 0;
    virtual void SetBaudrated(uint32_t baud) = 0;
    virtual void StartListenerMode() = 0;
    virtual void Stop() = 0;
    virtual void RegisterCanListener(CanListenerInterface *iface) = 0;
    virtual int ReadCanFrame(can_frame_type *frame) = 0;
};
