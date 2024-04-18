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
#include <stdio.h>
#include <stm32f4xx_map.h>
#include <uart.h>
#include <fwapi.h>
#include <fwobject.h>
#include <TimerInterface.h>
#include "app_tasks.h"

struct TimerCallbackType : public FwList {
    TimerListenerInterface *listener;
    uint64_t tm;
};


portTASK_FUNCTION(task1ms, args)
{
    FwList *p = fw_get_objects_list();
    FwObject *obj;
    CommonInterface *iface;
    TimerCallbackType *item;
    FwList *listener = 0;
    uint64_t tickcnt = 0;


    while (p) {
        obj = static_cast<FwObject *>(fwlist_get_payload(p));
        iface = obj->GetInterface("TimerListenerInterface");
        if (iface) {
            item = reinterpret_cast<TimerCallbackType *>(
                    fw_malloc(sizeof(TimerCallbackType)));

            item->listener =
                static_cast<TimerListenerInterface *>(iface);
            item->tm = item->listener->getTimerInterval();
            fwlist_add(&listener, item);
        }
        p = p->next;
    }

    while (1) {
        tickcnt++;
        item = static_cast<TimerCallbackType *>(listener);
        while (item) {
            if (tickcnt >= item->tm) {
                item->listener->callbackTimer(tickcnt);
                item->tm = tickcnt + item->listener->getTimerInterval();
            }
            item = static_cast<TimerCallbackType *>(item->next);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

