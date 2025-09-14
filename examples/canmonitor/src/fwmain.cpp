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

#if 1
#include <vprintfmt.h>

void show_int(int val, int line, int col) {
    // Length of "Watering in:" = 12. We have 8 symbol for numbers
    char tstr1[21];
    char tstr[21];
    int szmax = 20 - col;
    int sz = snprintf_lib(tstr1, szmax + 1, "%d", val);
    for (int i = 0; i < szmax - sz; i++) {
         tstr[i] = ' ';
    }
    for (int i = 0; i < sz; i++) {
         tstr[szmax - sz + i] = tstr1[i];
    }
    tstr[szmax] = 0;
    //display_outputText24Line("-=  1234", line, col, 0xffff, 0x0000); // clear number field
}
#endif

extern "C" int fwmain(int argcnt, char *args[]) {
    TaskHandle_t handleTask1ms;
    TaskHandle_t handleTaskEpoch;

    fw_init();
    show_int(15, 3, 12);

    EnableIrqGlobal();

    uart_printf("%s\n", "Starting FreeRTOS scheduler!\r\n");

    xTaskCreate(task1ms,
                 APP_TASK_NAME,
                 512,
                 NULL,
                 tskIDLE_PRIORITY + 2UL,
                 &handleTask1ms);

    xTaskCreate(taskEpoch,
                 APP_TASK_NAME,
                 1024,
                 NULL,
                 tskIDLE_PRIORITY + 1UL,
                 &handleTaskEpoch);

    vTaskStartScheduler();

    // NEVER REACH THIS CODE
    while(1) {
        uart_printf("FreerRTOS failed %s %s\r\n", __FILE__, __LINE__);
    }
    return 0;
}

