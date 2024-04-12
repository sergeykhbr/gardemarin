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

#include <fwattribute.h>
#include <fwlist.h>

class FwObject : public CommonInterface {
 public:
    explicit FwObject(const char *name);

    /**
     * @brief Initialize object generic method. Any parent FwObject should
     *        override this method to implement initialization code called from
     *        firmware kernel initialization procedure.
     */
    virtual void Init() {}

    /**
     * @brief Post Initialize stage. Can be used for debug output or similar.
     */
    virtual void PostInit() {}

    /**
     * @brief Get list of all registered attributes of this object.
     * @return Pointer to a start element of the single linked list of
     *         attributes.
     */
    FwList *GetAttributes() { return attrlist_; }

    /**
     * @brief Request to modify attribute from external interface (CAN for 
     *        an example). If FwObject supports writable attributes it should
     *        implement this method otherwise leave it empty
     * @param[in] dst Pointer to the attribute that should be modified
     * @param[in] src Pointer to the new attribute value
     */
    virtual void ModifyAttribute(FwAttribute *dst, FwAttribute *src) {}

    /**
     * @brief Get interface pointer by its name
     * @param[in] name Name of the requesting interface
     * @return Pointer to CommonInterface that can be statically cast into
     *         child interface. If requested interface wasn't registered for
     *         this class then return zero
     */
    CommonInterface *GetInterface(const char *name);

    CommonInterface *GetPortInterface(const char *portname, const char *name);

    /*
     * @brief Get object name
     * @return Pointer to an object name string
     */
    const char *ObjectName() { return objname_; }

 protected:
    /**
     * @brief Write attribtue into attribute list of the current object
     * @param[in] attr Pointer to CommonInterface tha can be statically case
                  into FwAttribute pointer.
     */
    void RegisterAttribute(CommonInterface *attr);

    /**
     * @brief Write interface into interface list of the current object
     * @param[in] iface Pointer to CommonInterface tha can be statically case
                  into and inherited from CommonInterface other interface.
     */
    void RegisterInterface(CommonInterface *iface);

    void RegisterPortInterface(const char *portname, CommonInterface *iface);

 protected:
    /**
     * @brief Object name in human readable format. Any module can request
     *        pointer to FwObject using this string identificator.
     */
    const char *objname_;

    /**
     * @brief Entry element pointer to a single linked list of all registered
              attributes of this class
     */
    FwList *attrlist_;

    /**
     * @brief Entry element pointer to a single linked list of all registered
              inteface of this class
     */
    FwList *ifacelist_;

    /**
     * @brief Entry element point of registered port intefaces
     */
    FwList *portlist_;
};
