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
#include <string.h>

/**
 * @brief Kernel module constructor. Instance of this KernelClass registered
 *        as the first element in the single linked list of FwObjects.
 */
KernelClass::KernelClass(const char *name) : FwObject(name),
    targetConfig_(this, "TargetConfig"),
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
    adc1_("adc1"),
    hbrg0_("hbrg0", 0),
    hbrg1_("hbrg1", 1),
    hbrg2_("hbrg2", 2),
    hbrg3_("hbrg3", 3),
    temp0_("temp0"),
    dbc_("dbc")
{
    version_.make_uint32(0x20240804);
}

/**
 * @brief Overrided FwObject method to register attribtues and interface
 */
void KernelClass::Init() {
    RegisterAttribute(&targetConfig_);
    RegisterAttribute(&version_);
}

/**
 * @brief Debug output of the Kernel configuration
 */
void KernelClass::PostInit() {
    targetConfig_.to_string();
}

void KernelClass::TargetConfigAttribute::pre_read() {
FwObject *obj;
    FwAttribute *attr;
    FwList *p = fw_get_objects_list();
    FwList *alist;
    int obj_idx;
    int atr_idx;

    uart_printf("'TargetConfig':{\r\n"
                "    'ObjectsList':[\r\n");
    obj_idx = 0;
    while (p) {
        obj = static_cast<FwObject *>(fwlist_get_payload(p));
        uart_printf("       {'Index':%d, 'Name':'%s',\r\n",
                    obj_idx, obj->ObjectName());
        uart_printf("        'Attributes':[");
        alist = obj->GetAttributes();
        atr_idx = 0;
        if (alist) {
            uart_printf("\r\n");
        } else {
            uart_printf("]\r\n");
        }
        while (alist) {
            attr = (FwAttribute *)fwlist_get_payload(alist);
            print_attribute(atr_idx, attr);

            alist = alist->next;
            if (alist) {
                uart_printf(",\r\n");
            } else {
                uart_printf("]\r\n");
            }
            atr_idx++;
        }
        p = p->next;
        if (p) {
            uart_printf("       },\r\n");
        } else {
            uart_printf("   }]\r\n");
        }
        obj_idx++;
    }
    uart_printf("}\r\n");
}

void KernelClass::TargetConfigAttribute::print_attribute(int idx,
                                                FwAttribute *attr) {
    int t1;
    uart_printf("           {'Index':%d", idx);
    uart_printf(", 'Name':'%s'", attr->name());
    uart_printf(", 'Type':'%s'", KindTypeString[attr->kind()]);
    uart_printf(", 'Value':", KindTypeString[attr->kind()]);
    switch (attr->kind()) {
    case Attr_String:
        if (strcmp(attr->name(), name()) == 0) {
            // exclude this attribute from the output list
            uart_printf("''");
        } else {
            uart_printf("'%s'", attr->to_string());
        }
        break;
    case Attr_Int8:
        uart_printf("%d", attr->to_int8());
        break;
    case Attr_UInt8:
        uart_printf("%02x", attr->to_uint8());
        break;
    case Attr_Int16:
        uart_printf("%d", attr->to_int16());
        break;
    case Attr_UInt16:
        uart_printf("0x%04x", attr->to_uint16());
        break;
    case Attr_Int32:
        uart_printf("%ld", attr->to_int32());
        break;
    case Attr_UInt32:
        uart_printf("0x%08lx", attr->to_uint32());
        break;
    case Attr_Int64:
        uart_printf("%" RV_PRI64 "d", attr->to_int64());
        break;
    case Attr_UInt64:
        uart_printf("%" RV_PRI64 "x", attr->to_uint64());
        break;
    case Attr_Float:
        t1 = static_cast<int>(attr->to_float());
        uart_printf("%d.%06d", t1,
            static_cast<int>(1000000.0f * (attr->to_float() - t1)));
        break;
    case Attr_Double:
        uart_printf("%.2f", attr->to_double());
        break;
    default:;
    }
    uart_printf(", 'Descr':'%s'}", attr->description());
}