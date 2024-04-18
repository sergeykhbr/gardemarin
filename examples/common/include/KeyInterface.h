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

class KeyListenerInterface : public CommonInterface {
 public:
    KeyListenerInterface() : CommonInterface("KeyListenerInterface") {}

    virtual void keyPressed() = 0;
    virtual void keyReleased() = 0;
    virtual void keyClick() = 0;
    virtual void keyDoubleClick() = 0;
    virtual void keyLongClick() = 0;
};


class KeyInterface : public CommonInterface {
 public:
    KeyInterface() : CommonInterface("KeyInterface") {}

    virtual void registerKeyListener(KeyListenerInterface *iface) = 0;
};
