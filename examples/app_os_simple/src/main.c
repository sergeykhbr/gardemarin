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

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>

static portTASK_FUNCTION_PROTO(vLEDFlashTask, pvParameters);

int main(int argcnt, char *args[]) {
    //fw_malloc_init();

    printf("%s\n", "Starting FreeRTOS scheduler!");

    xTaskCreate(vLEDFlashTask,
                "LEDx",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 tskIDLE_PRIORITY + 1UL,
                 (TaskHandle_t *) NULL);

    vTaskStartScheduler();

    // NEVER REACH THIS CODE
    while(1) {}
    return 0;
}

static portTASK_FUNCTION(vLEDFlashTask, pvParameters)
{
    while (1) {
        // do something
    }
}

