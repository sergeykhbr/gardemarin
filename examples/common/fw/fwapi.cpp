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

#include <prjtypes.h>
#include <fwobject.h>
#include <RawInterface.h>
#include <KernelInterface.h>
#include <fwapi.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Get KernelInterface interface pointer that should return single
 *        kernel instance. In theory that should be the only single object
 *        in the firmware
 */
extern "C" KernelInterface *GetKernelInterface();

extern "C" void fw_init() {
    KernelInterface *kernel = GetKernelInterface();
    FwList *p = kernel->GetObjectList();
    FwObject *obj;

    // Init stage:
    while (p) {
        obj = static_cast<FwObject *>(fwlist_payload(p));
        obj->Init();
        p = p->next;
    }

    // PostInit stage:
    p = kernel->GetObjectList();
    while (p) {
        obj = static_cast<FwObject *>(fwlist_payload(p));
        obj->PostInit();
        p = p->next;
    }
}

extern "C" FwList *fw_empty_list_item() {
    return GetKernelInterface()->GetEmptyListItem();
}

/**
 * @brief register new FwObject casted tp CommonInterface
 * @param[in] obj Pointer that should be registered in the linked list of
 *                objects.
 * @todo Implement alphabetically sorted list to improve list search
 */
extern "C" void fw_register_object(void *obj) {
    GetKernelInterface()->RegisterObject(static_cast<FwObject *>(obj));
}

/**
 * @brief Get single linked list of all registered objects
 * @return Pointer to list of regsitered objects. At least one should be
 *         presence as Kernel object.
 */
extern "C" FwList *fw_get_objects_list() {
    return GetKernelInterface()->GetObjectList();
}

/**
 * @brief Get pointer to FwObject by its name.
 * @param[i] name Object name string used as the FwObject identificator.
 * @return Pointer to FwObject in a case of success or zero value if the
 *         the object not found
 */
extern "C" CommonInterface *fw_get_object(const char *name) {
    FwObject *obj;
    FwList *p = GetKernelInterface()->GetObjectList();
    while (p) {
        obj = (FwObject *)fwlist_payload(p);
        p = p->next;
        if (strcmp(obj->ObjectName(), name) != 0) {
            obj = 0;
            continue;
        }
        break;
    }
    return obj;
}

/**
 * @brief Get pointer to the specified interface of the specified FwObject.
 * @param[i] objname Object name string used as the FwObject identificator.
 * @param[i] facename Interface name.
 * @return Pointer to CommonInterface if the specified interface was found
 *         or zero value otherwise
 */
extern "C" CommonInterface *fw_get_object_interface(const char *objname,
                                         const char *facename) {
    FwObject *obj;
    CommonInterface *ret = 0;
    obj = static_cast<FwObject *>(fw_get_object(objname));
    if (obj) {
        ret = obj->GetInterface(facename);
    }
    return ret;
}

/**
 * @brief Get FwObject interface using DBC index corresponding to
 *        index in object list
 * @param[in] obj_idx Index of FwObject in the object list
 * @return Interface to FwObject in case if this index exists or zero
 *         if object not found.
 */
extern "C" CommonInterface *fw_get_obj_by_index(int obj_idx) {
    int tcnt = 0;
    FwList *pitem = GetKernelInterface()->GetObjectList();
    CommonInterface *ret = 0;
    // Find object with the specified index
    while (pitem != 0 && tcnt != obj_idx) {
        pitem = pitem->next;
        tcnt++;
    }

    if (pitem != 0 && tcnt == obj_idx) {
        ret = fwlist_payload(pitem);
    }
    return ret;
}

/**
 * @brief Get FwAttribute interface using DBC index corresponding to
 *        index in attribute list
 * @param[in] obj Pointer to FwObject interface
 * @param[in] atr_idx Index of FwAttribute in the list of object's attributes
 * @return Interface to FwAttribute in case if this index exists or zero
 *         if attribute not found.
 */
extern "C" CommonInterface *fw_get_attr_by_index(CommonInterface *obj,
                                                 int atr_idx) {
    int tcnt = 0;
    FwList *pitem = static_cast<FwObject *>(obj)->GetAttributes();
    CommonInterface *ret = 0;
    // Find object with the specified index
    while (pitem != 0 && tcnt != atr_idx) {
        pitem = pitem->next;
        tcnt++;
    }

    if (pitem != 0 && tcnt == atr_idx) {
        ret = fwlist_payload(pitem);
    }
    return ret;
}

