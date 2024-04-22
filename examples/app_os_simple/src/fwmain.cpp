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
#include <fwapi.h>
#include <uart.h>
#include <new>
#include "app_tasks.h"

extern "C" int fwmain(int argcnt, char *args[]) {
    app_data_type *appdata;

    fw_init();

    appdata = (app_data_type *)fw_get_ram_data(APP_TASK_NAME);
    if (appdata == 0) {
        appdata = (app_data_type *)fw_malloc(sizeof(app_data_type));
        //appdata = pvPortMalloc(sizeof(app_data_type));
        memset(appdata, 0, sizeof(app_data_type));
        appdata->keyNotifier = 
                new(fw_malloc(sizeof(KeyNotifierType))) KeyNotifierType();
        appdata->keyNotifier->data = appdata;
        appdata->keyNotifier->btnClick = false;

        fw_register_ram_data(APP_TASK_NAME, appdata);
    }

    EnableIrqGlobal();

    uart_printf("%s\n", "Starting FreeRTOS scheduler!\r\n");

    xTaskCreate(task1ms,
                 APP_TASK_NAME,
                 configMINIMAL_STACK_SIZE,
                 appdata,
                 tskIDLE_PRIORITY + 1UL,
                 &appdata->handleTask1ms);

    xTaskCreate(task500ms,
                 APP_TASK_NAME,
                 2*configMINIMAL_STACK_SIZE,
                 appdata,
                 tskIDLE_PRIORITY + 1UL,
                 &appdata->handleTask500ms);

    vTaskStartScheduler();

    // NEVER REACH THIS CODE
    while(1) {
        uart_printf("FreerRTOS failed %s %s\r\n", __FILE__, __LINE__);
    }
    return 0;
}

