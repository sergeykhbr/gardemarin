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

#include <inttypes.h>
#include <fwlist.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Main initialzation method before fw-methods becomes available
 */
void fw_init();

void fw_malloc_init();

void *fw_malloc(int size);

void fw_register_ram_data(const char *name, void *data);

void *fw_get_ram_data(const char *name);

/**
 * @brief Get pointer to an empty FwList element. FW allocates items
 *        from the static array of the fixed size and check that number
 *        allocated items do not exceed maximum value.
 * @return Pointer to an empty list item.
 */
FwList *fw_empty_list_item();

/**
 * @brief register new FwObject casted tp CommonInterface
 * @param[in] obj Pointer to FwObject.
 */
void fw_register_object(void *obj);

/**
 * @brief Get single linked list of all registered objects
 * @return Pointer to list of regsitered objects. At least one should be
 *         presence as Kernel object.
 */
FwList *fw_get_objects_list();

/**
 * @brief Get pointer to FwObject by its name.
 * @param[i] name Object name string used as the FwObject identificator.
 * @return Pointer to FwObject in a case of success or zero value if the
 *         the object not found
 */
void *fw_get_object(const char *name);

/**
 * @brief Get pointer to the specified interface of the specified FwObject.
 * @param[i] objname Object name string used as the FwObject identificator.
 * @param[i] facename Interface name.
 * @return Pointer to CommonInterface if the specified interface was found
 *         or zero value otherwise
 */
void *fw_get_object_interface(const char *objname,
                              const char *facename);

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
                                   const char *facename);

/**
 * @brief Get FwObject interface using DBC index corresponding to
 *        index in object list
 * @param[in] obj_idx Index of FwObject in the object list
 * @return Interface to FwObject in case if this index exists or zero
 *         if object not found.
 */
void *fw_get_obj_by_index(int obj_idx);


/**
 * @brief Get FwAttribute interface
 * @param[in] obj Pointer to FwObject interface
 * @param[in] atr_idx Index of FwAttribute in the list of object's attributes
 * @return Interface to FwAttribute in case if this index exists or zero
 *         if attribute not found.
 */
void *fw_get_obj_attr_by_name(void *obj, const char *atrname);

/**
 * @brief Get FwAttribute interface using DBC index corresponding to
 *        index in attribute list
 * @param[in] obj Pointer to FwObject interface
 * @param[in] atr_idx Index of FwAttribute in the list of object's attributes
 * @return Interface to FwAttribute in case if this index exists or zero
 *         if attribute not found.
 */
void *fw_get_obj_attr_by_index(void *obj, int atr_idx);


#ifdef __cplusplus
}
#endif
