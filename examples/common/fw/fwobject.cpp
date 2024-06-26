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
#include <string.h>
#include <fwobject.h>
#include <uart.h>

struct FwPortType : public FwList {
    const char *portname;
};

FwObject::FwObject(const char *name) : CommonInterface("FwObject"),
    objname_(name),
    attrlist_(0),
    ifacelist_(0),
    portlist_(0)
{
    fw_register_object(static_cast<FwObject *>(this));
}

void FwObject::RegisterAttribute(CommonInterface *attr) {
    FwList *pnew = fw_empty_list_item();
    fwlist_set_payload(pnew, attr);
    fwlist_add(&attrlist_, pnew);
}

void FwObject::RegisterInterface(CommonInterface *attr) {
    FwList *pnew = fw_empty_list_item();
    fwlist_set_payload(pnew, attr);
    fwlist_add(&ifacelist_, pnew);
}

void FwObject::RegisterPortInterface(const char *portname, CommonInterface *attr) {
    FwPortType *pnew = reinterpret_cast<FwPortType *>(fw_malloc(sizeof(FwPortType)));
    pnew->portname = portname;
    fwlist_set_payload(pnew, attr);
    fwlist_add(&portlist_, pnew);
}

CommonInterface *FwObject::GetInterface(const char *name) {
    CommonInterface *ret = 0;
    FwList *p = ifacelist_;
    while (p) {
        ret = reinterpret_cast<CommonInterface *>(fwlist_get_payload(p));
        if (strcmp(ret->GetFaceName(), name) != 0) {
            ret = 0;
            p = p->next;
            continue;
        }
        break;
    }
    return ret;
}

CommonInterface *FwObject::GetPortInterface(const char *portname, const char *name) {
    CommonInterface *ret = 0;
    FwList *p = portlist_;
    while (p) {
        ret = reinterpret_cast<CommonInterface *>(fwlist_get_payload(p));
        if (strcmp(ret->GetFaceName(), name) != 0
           || strcmp(static_cast<FwPortType *>(p)->portname, portname) != 0) {
            ret = 0;
            p = p->next;
            continue;
        }
        break;
    }
    return ret;
}
