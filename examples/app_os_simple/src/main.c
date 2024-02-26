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
#include <string.h>
#include <stm32f4xx_map.h>
#include "task500ms.h"


typedef struct app_data_type  {
    task500ms_data_type task500ms_arg;
} app_data_type;

int main(int argcnt, char *args[]) {
    app_data_type *appdata;

    printf("%s\n", "Starting FreeRTOS scheduler!");

    appdata = pvPortMalloc(sizeof(app_data_type));
    memset(appdata, 0, sizeof(app_data_type));

    xTaskCreate(task500ms,
                "10ms",
                 configMINIMAL_STACK_SIZE,
                 &appdata->task500ms_arg,
                 tskIDLE_PRIORITY + 1UL,
                 (TaskHandle_t *) NULL);

    vTaskStartScheduler();

    // NEVER REACH THIS CODE
    while(1) {
        printf("FreerRTOS failed %s %s\n", __FILE__, __LINE__);
    }
    return 0;
}

