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

FwObject::FwObject(const char *name) : CommonInterface("FwObject"),
    objname_(name),
    attrlist_(0),
    ifacelist_(0)
{
}

void FwObject::RegisterAttribute(CommonInterface *attr) {
    FwList *pnew = fw_empty_list_item();
    fwlist_init(pnew, attr);
    if (attrlist_ == 0) {
        attrlist_ = pnew;
    } else {
        fwlist_add(attrlist_, pnew);
    }
}

void FwObject::RegisterInterface(CommonInterface *attr) {
    FwList *pnew = fw_empty_list_item();
    fwlist_init(pnew, attr);
    if (ifacelist_ == 0) {
        ifacelist_ = pnew;
    } else {
        fwlist_add(ifacelist_, pnew);
    }
}

CommonInterface *FwObject::GetInterface(const char *name) {
    CommonInterface *ret = 0;
    FwList *p = ifacelist_;
    while (p) {
        ret = (CommonInterface *)fwlist_payload(p);
        if (strcmp(ret->GetFaceName(), name) != 0) {
            ret = 0;
            p = p->next;
            continue;
        }
        break;
    }
    return ret;
}
