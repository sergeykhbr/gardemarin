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
#include <fwapi.h>
#include <fwkernel.h>
#include <string.h>
#include <stdio.h>
#include <new>
#include <uart.h>

FwList *objlist_ = 0;

extern "C" void fw_init() {
    FwList *p;
    FwObject *obj;

    fw_malloc_init();

uart_printf("%s,%d\r\n", __FILE__, __LINE__);
    new (fw_malloc(sizeof(KernelClass))) KernelClass("kernel");

uart_printf("%s,%d\r\n", __FILE__, __LINE__);
    // It is possible to create JSON-configration file and platoform loading here
    p = fw_get_objects_list();
uart_printf("%s,%d\r\n", __FILE__, __LINE__);
    while (p) {
        obj = reinterpret_cast<FwObject *>(fwlist_get_payload(p));
        obj->Init();
        p = p->next;
    }

uart_printf("%s,%d\r\n", __FILE__, __LINE__);
    p = fw_get_objects_list();
    while (p) {
        obj = reinterpret_cast<FwObject *>(fwlist_get_payload(p));
        obj->PostInit();
        p = p->next;
    }
uart_printf("%s,%d\r\n", __FILE__, __LINE__);
}

extern "C" FwList *fw_empty_list_item() {
    FwList *ret = (FwList *)fw_malloc(sizeof(FwList));

    fwlist_set_payload(ret, 0);
    return ret;
}

/**
 * @brief register new FwObject casted tp CommonInterface
 * @param[in] obj Pointer that should be registered in the linked list of
 *                objects.
 * @todo Implement alphabetically sorted list to improve list search
 */
extern "C" void fw_register_object(void *obj) {
    FwList *p = fw_empty_list_item();
    
    fwlist_set_payload(p, obj);
    fwlist_add(&objlist_, p);
}

/**
 * @brief Get single linked list of all registered objects
 * @return Pointer to list of regsitered objects. At least one should be
 *         presence as Kernel object.
 */
extern "C" FwList *fw_get_objects_list() {
    return objlist_;
}

/**
 * @brief Get pointer to FwObject by its name.
 * @param[i] name Object name string used as the FwObject identificator.
 * @return Pointer to FwObject in a case of success or zero value if the
 *         the object not found
 */
extern "C" void *fw_get_object(const char *name) {
    FwObject *obj;
    FwList *p = fw_get_objects_list();
    while (p) {
        obj = reinterpret_cast<FwObject *>(fwlist_get_payload(p));
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
extern "C" void *fw_get_object_interface(const char *objname,
                                         const char *facename) {
    FwObject *obj;
    CommonInterface *ret = 0;
    obj = reinterpret_cast<FwObject *>(fw_get_object(objname));
    if (obj) {
        ret = obj->GetInterface(facename);
    }
    return ret;
}

/**
 * @brief Get pointer to the specified interface of the specified FwObject.
 *        when the object implements several interfaces of the same type
 * @param[i] objname Object name string used as the FwObject identificator.
 * @param[i] portname Port name to witch the interface is bound.
 * @param[i] facename Interface name.
 * @return Pointer to CommonInterface if the specified interface was found
 *         or zero value otherwise
 */
void *fw_get_object_port_interface(const char *objname,
                                   const char *portname,
                                   const char *facename) {
    FwObject *obj;
    CommonInterface *ret = 0;
    obj = reinterpret_cast<FwObject *>(fw_get_object(objname));
    if (obj) {
        ret = obj->GetPortInterface(portname, facename);
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
extern "C" void *fw_get_obj_by_index(int obj_idx) {
    int tcnt = 0;
    FwList *pitem = fw_get_objects_list();
    FwObject *ret = 0;
    // Find object with the specified index
    while (pitem != 0 && tcnt != obj_idx) {
        pitem = pitem->next;
        tcnt++;
    }

    if (pitem != 0 && tcnt == obj_idx) {
        ret = reinterpret_cast<FwObject *>(fwlist_get_payload(pitem));
    }
    return ret;
}

extern "C" void *fw_get_obj_attr_by_name(void *obj, const char *atrname) {
    FwAttribute *attr = 0;
    FwList *pitem = reinterpret_cast<FwObject *>(obj)->GetAttributes();
    // Find object with the specified index
    while (pitem) {
        attr = reinterpret_cast<FwAttribute *>(fwlist_get_payload(pitem));
        if (strcmp(attr->name(), atrname) == 0) {
            return attr;
        }
        pitem = pitem->next;
    }

    return 0;
}

/**
 * @brief Get FwAttribute interface using DBC index corresponding to
 *        index in attribute list
 * @param[in] obj Pointer to FwObject interface
 * @param[in] atr_idx Index of FwAttribute in the list of object's attributes
 * @return Interface to FwAttribute in case if this index exists or zero
 *         if attribute not found.
 */
extern "C" void *fw_get_obj_attr_by_index(void *obj,
                                      int atr_idx) {
    int tcnt = 0;
    FwList *pitem = reinterpret_cast<FwObject *>(obj)->GetAttributes();
    void *ret = 0;
    // Find object with the specified index
    while (pitem != 0 && tcnt != atr_idx) {
        pitem = pitem->next;
        tcnt++;
    }

    if (pitem != 0 && tcnt == atr_idx) {
        ret = fwlist_get_payload(pitem);
    }
    return ret;
}

