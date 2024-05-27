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
#include <new>
#include <fwapi.h>
#include <fwobject.h>
#include "app_tasks.h"
#include "ManagementClass.h"

// current task is 0.5 sec
#define SERVICE_SEC_TO_COUNT(sec) (2 * sec)

portTASK_FUNCTION(taskEpoch, args)
{
    TaskHandle_t taskHandle = xTaskGetCurrentTaskHandle();
    const TickType_t delay_ms = 1000 / portTICK_PERIOD_MS;

    ManagementClass *epochClass_ = 
        new (fw_malloc(sizeof(ManagementClass))) ManagementClass(taskHandle);

    epochClass_->Init();
    epochClass_->PostInit();

    while (1) {
        // do something
        epochClass_->update();

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

