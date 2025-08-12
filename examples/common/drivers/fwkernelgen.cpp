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
#include "fwkernelgen.h"
#include <string.h>

/**
 * @brief Kernel module constructor. Instance of this KernelClass registered
 *        as the first element in the single linked list of FwObjects.
 */
KernelClassGeneric::KernelClassGeneric(const char *name) : FwObject(name),
    targetConfig_(this, "TargetConfig"),
    version_("Version"),
    output_(this, "Output"),
    uart1_("uart1"),
    rtc_("rtc"),
    dbc_("dbc")
{
    version_.make_uint32(0x20240804);
    output_.make_int32(0);
}

/**
 * @brief Overrided FwObject method to register attribtues and interface
 */
void KernelClassGeneric::Init() {
    RegisterAttribute(&targetConfig_);
    RegisterAttribute(&version_);
    RegisterAttribute(&output_);
}

/**
 * @brief Debug output of the Kernel configuration
 */
void KernelClassGeneric::PostInit() {
    targetConfig_.to_string();
}

void KernelClassGeneric::addToOutputList(FwAttribute *attr, uint32_t period) {
    
}

void KernelClassGeneric::removeFromOutputList(FwAttribute *attr) {
}


void KernelClassGeneric::TargetConfigAttribute::pre_read() {
FwObject *obj;
    FwAttribute *attr;
    FwList *p = fw_get_objects_list();
    FwList *alist;
    int obj_idx;
    int atr_idx;

    uart_printk("'TargetConfig':{\r\n"
                "    'ObjectsList':[\r\n");
    obj_idx = 0;
    while (p) {
        obj = static_cast<FwObject *>(fwlist_get_payload(p));
        uart_printk("       {'Index':%d, 'Name':'%s',\r\n",
                    obj_idx, obj->ObjectName());
        uart_printk("        'Attributes':[");
        alist = obj->GetAttributes();
        atr_idx = 0;
        if (alist) {
            uart_printk("\r\n");
        } else {
            uart_printk("]\r\n");
        }
        while (alist) {
            attr = (FwAttribute *)fwlist_get_payload(alist);
            print_attribute(atr_idx, attr);

            alist = alist->next;
            if (alist) {
                uart_printk(",\r\n");
            } else {
                uart_printk("]\r\n");
            }
            atr_idx++;
        }
        p = p->next;
        if (p) {
            uart_printk("       },\r\n");
        } else {
            uart_printk("   }]\r\n");
        }
        obj_idx++;
    }
    uart_printk("}\r\n");
}

void KernelClassGeneric::TargetConfigAttribute::print_attribute(int idx,
                                                                FwAttribute *attr) {
    int t1;
    float f1;
    uart_printk("           {'Index':%d", idx);
    uart_printk(", 'Name':'%s'", attr->name());
    uart_printk(", 'Type':'%s'", KindTypeString[attr->kind()]);
    uart_printk(", 'Value':", KindTypeString[attr->kind()]);
    switch (attr->kind()) {
    case Attr_String:
        if (strcmp(attr->name(), name()) == 0) {
            // exclude this attribute from the output list
            uart_printk("''");
        } else {
            uart_printk("'%s'", attr->to_string());
        }
        break;
    case Attr_Int8:
        uart_printk("%d", attr->to_int8());
        break;
    case Attr_UInt8:
        uart_printk("%02x", attr->to_uint8());
        break;
    case Attr_Int16:
        uart_printk("%d", attr->to_int16());
        break;
    case Attr_UInt16:
        uart_printk("0x%04x", attr->to_uint16());
        break;
    case Attr_Int32:
        uart_printk("%ld", attr->to_int32());
        break;
    case Attr_UInt32:
        uart_printk("0x%08lx", attr->to_uint32());
        break;
    case Attr_Int64:
        uart_printk("%" RV_PRI64 "d", attr->to_int64());
        break;
    case Attr_UInt64:
        uart_printk("%" RV_PRI64 "x", attr->to_uint64());
        break;
    case Attr_Float:
        f1 = attr->to_float();
        if (f1 < 0) {
            f1 = -f1;
        }
        t1 = static_cast<int>(f1);
        if (attr->to_float() < 0) {
            uart_printk("-");
        }
        uart_printk("%d.%06d", t1,
            static_cast<int>(1000000.0f * (f1 - t1)));
        break;
    case Attr_Double:
        uart_printk("%.2f", attr->to_double());
        break;
    default:;
    }
    uart_printk(", 'Descr':'%s'}", attr->description());
}

void KernelClassGeneric::OutputControlAttribute::post_write() {
    FwObject *obj;
    FwAttribute *attr;
    ValueType ctrl;
    ctrl.u32 = u_.u32;

    obj = reinterpret_cast<FwObject *>(fw_get_obj_by_index(ctrl.b.objid));
    if (obj == 0) {
        return;
    }
    attr = reinterpret_cast<FwAttribute *>(fw_get_obj_attr_by_index(obj, ctrl.b.attrid));
    if (attr == 0) {
        return;
    }

    if (ctrl.b.ena_dis) {
        parent_->addToOutputList(attr, ctrl.b.period);
    } else {
        parent_->removeFromOutputList(attr);
    }

}

