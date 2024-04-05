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

#include <CommonInterface.h>

/**
 * @brief Singly linked list common structure
 */
typedef struct FwList {
    FwList *next;
    CommonInterface *payload;
} FwList;

/**
 * @brief Initialize common list structure
 * @param[in] p       Pointer to structure that should be initialized
 * @param[in] payload Pointer to the data payload
 */
static inline void fwlist_init(FwList *p, CommonInterface *payload) {
    p->next = 0;
    p->payload = payload;
}

/**
 * @brief Read payload data from the common list structure
 * @param[in] p Pointer to structure from which data should be read
 * @return      Pointer to the allocated CommonInterface stored as payload
 */
static inline CommonInterface *fwlist_payload(FwList *p) {
    return p->payload;
}

/**
 * @brief Add newly created common list structure into the linked list
 * @param[in] pfirst Pointer to the first structure of the single linked list
 * @param[in] pnew   Pointer to the structure adding into list
 */
static inline void fwlist_add(FwList *first, FwList *pnew) {
    FwList *last = first;
    while (last->next) {
        last = last->next;
    }
    pnew->next = 0;
    last->next = pnew;
}
