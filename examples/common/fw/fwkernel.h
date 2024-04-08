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

#include <fwlist.h>
#include <fwobject.h>
#include <FwAttribute.h>
#include <KernelInterface.h>

class KernelClass : public FwObject,
                    public KernelInterface {
 public:
    KernelClass();

    // FwObject interface:
    virtual void Init() override;
    virtual void PostInit() override;

    // KernelInterface interface:
    virtual FwList *GetObjectList() override { return objlist_; }
    virtual FwList *GetEmptyListItem() override ;
    virtual void RegisterObject(FwList *pnew) override;

 private:
    /**
     * @brief Maximum number of FwObject instances in the whole FW
     * @note It is better use heap (new/malloc) memory region but it is should
     *       be checked with the MISRA rules.
     */
    static const int OBJECT_LIST_MAX = 32;

    /**
     * @brief Point to the first item of the single linked list of
     * registered objects
     */
    FwList *objlist_;

    /** 
     * @brief Statically allocated list of items used to implement single linked
     *        lists. Types of lists:
     *          - list of FwObjects
     *          - list of FwAttributes for each FwObject (if presence)
     *          - list of additional Interfaces for each FwObject (if presence)
     */
    FwList list_array_[OBJECT_LIST_MAX];

    /** @brief Kernel Version attribute */
    FwAttribute version_;
    /** @brief Counter and index of the last empty List Item */
    FwAttribute listCnt_;
    /** @brief Statically allocated maximum number of the List Items */
    FwAttribute listMax_;

};
