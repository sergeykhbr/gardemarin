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

#include "FwObject.h"

class KernelInterface : public CommonInterface {
 public:
    KernelInterface() : CommonInterface("KernelInterface") {}

    /**
     * @brief Get list of all registered objects
     * @return Pointer to the first element of the single list element
     */
    virtual FwList *GetObjectList() = 0;

    /**
     * @brief Allocated new list item to add into one of linked lists
     */
    virtual FwList *GetEmptyListItem() = 0;

    /**
     * @brief Register new FwObject
     */
    virtual void RegisterObject(FwObject *obj) = 0;
};
