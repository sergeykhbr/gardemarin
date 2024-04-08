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


#include "fwkernel.h"
#include <uart.h>

/**
 * @brief Single tone variable that creates all other modules and drivers
 */
KernelClass kernel;

/**
 * @brief Access method to KernelInterface from API file. This method allows
 *        to avoid including of the KernelClass.h file and as result interact
 *        only using virtual interface classes.
 */
extern "C" KernelInterface *GetKernelInterface()
{
    return static_cast<KernelInterface *>(&kernel);
}

/**
 * @brief Kernel module constructor. Instance of this KernelClass registered
 *        as the first element in the single linked list of FwObjects.
 */
KernelClass::KernelClass() : FwObject("kernel"),
    version_("Version"),
    listCnt_("ListCnt"),
    listMax_("ListMax")
{
    // Add First element of the linked list to kernel itself
    objlist_ = &list_array_[0];
    objlist_->next = 0;
    objlist_->payload = static_cast<FwObject *>(this);

    version_.make_uint32(0x20240804);
    listCnt_.make_int16(1);
    listMax_.make_int16(OBJECT_LIST_MAX);
}

/**
 * @brief Overrided FwObject method to register attribtues and interface
 */
void KernelClass::Init() {
    RegisterAttribute(&version_);
    RegisterAttribute(&listCnt_);
    RegisterAttribute(&listMax_);
}

/**
 * @brief Debug output of the Kernel configuration
 */
void KernelClass::PostInit() {
    FwObject *obj;
    FwAttribute *attr;
    FwList *p = GetObjectList();
    FwList *alist;
    int obj_idx;
    int atr_idx;

    uart_printf("Registered objects list:\r\n");
    obj_idx = 0;
    while (p) {
        obj = static_cast<FwObject *>(fwlist_payload(p));
        uart_printf("  Attributes of class '%s':\r\n", obj->ObjectName());
        alist = obj->GetAttributes();
        atr_idx = 0;
        while (alist) {
            attr = (FwAttribute *)fwlist_payload(alist);
            uart_printf("    ID[%d,%d] => %s", obj_idx, atr_idx, attr->name());
            switch (attr->kind()) {
            case Attr_String:
                uart_printf(", string, %s", attr->to_string());
                break;
            case Attr_Int8:
                uart_printf(", int8, %d", attr->to_int8());
                break;
            case Attr_UInt8:
                uart_printf(", uint8, %d", attr->to_uint8());
                break;
            case Attr_Int16:
                uart_printf(", int16, %d", attr->to_int16());
                break;
            case Attr_UInt16:
                uart_printf(", uint16, 0x%04x", attr->to_uint16());
                break;
            case Attr_Int32:
                uart_printf(", int32, %ld", attr->to_int32());
                break;
            case Attr_UInt32:
                uart_printf(", uint32, 0x%08lx", attr->to_uint32());
                break;
            case Attr_Int64:
                uart_printf(", int64, %" RV_PRI64 "d", attr->to_int64());
                break;
            case Attr_UInt64:
                uart_printf(", uint64, %" RV_PRI64 "x", attr->to_uint64());
                break;
            case Attr_Float:
                uart_printf(", float, %.2f", attr->to_float());
                break;
            case Attr_Double:
                uart_printf(", double, %.2f", attr->to_double());
                break;
            default:;
            }
            if (attr->description()) {
                uart_printf(", %s", attr->description());
            }
            uart_printf("\r\n");
            alist = alist->next;
            atr_idx++;
        }
        p = p->next;
        obj_idx++;
    }
    uart_printf("\r\n");
}

/**
 * @brief Allocated new list item to add into one of linked lists
 * @return Pointer to an empty FwList structure. In a case if there's
 *         no empty item, infinite cycle with the continous debug output
 *         will be generated.
 */
FwList *KernelClass::GetEmptyListItem() {
    int cnt = listCnt_.to_int16();
    FwList *ret = &list_array_[cnt];
    if (cnt >= OBJECT_LIST_MAX) {
        while (1) {
            uart_printf("Increase FwList size OBJECT_LIST_MAX=%d\r\n",
                        OBJECT_LIST_MAX);
        }
    }
    listCnt_.make_int16(cnt + 1);
    return ret;
}

/**
 * @brief Register new FwObject as the payload of the FwList structure
 * param[in] pnew Pointer to an object that should be added into object
 *                single linked linst stored in the kernel.
 * @note Nice optimization is to implement sorting algorithm to speed-up
 *       list search.
 */
void KernelClass::RegisterObject(FwList *pnew) {
    if (objlist_ == 0) {
        objlist_ = pnew;
    } else {
        fwlist_add(objlist_, pnew);
    }
}
