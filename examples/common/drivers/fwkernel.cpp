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

#include <fwapi.h>
#include <uart.h>
#include "fwkernel.h"

/**
 * @brief Kernel module constructor. Instance of this KernelClass registered
 *        as the first element in the single linked list of FwObjects.
 */
KernelClass::KernelClass(const char *name) : FwObject(name),
    version_("Version"),
    uart1_("uart1"),
    relais0_("relais0", 0),
    relais1_("relais1", 1),
    ledrbw_("ledrbw"),
    can1_("can1", 0),
    can2_("can2", 1),
    scales_("scales"),
    uled0_("uled0"),
    ubtn0_("ubtn0"),
    hbrg0_(0),
    hbrg1_(1),
    hbrg2_(2),
    hbrg3_(3),
    dbc_("dbc")
{
    version_.make_uint32(0x20240804);
}

/**
 * @brief Overrided FwObject method to register attribtues and interface
 */
void KernelClass::Init() {
    RegisterAttribute(&version_);
}

/**
 * @brief Debug output of the Kernel configuration
 */
void KernelClass::PostInit() {
    FwObject *obj;
    FwAttribute *attr;
    FwList *p = fw_get_objects_list();
    FwList *alist;
    int obj_idx;
    int atr_idx;

    uart_printf("Registered objects list:\r\n");
    obj_idx = 0;
    while (p) {
        obj = static_cast<FwObject *>(fwlist_get_payload(p));
        uart_printf("  Attributes of class '%s':\r\n", obj->ObjectName());
        alist = obj->GetAttributes();
        atr_idx = 0;
        while (alist) {
            attr = (FwAttribute *)fwlist_get_payload(alist);
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

