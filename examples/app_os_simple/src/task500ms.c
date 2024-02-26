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
#include "task500ms.h"

static int is_button_pressed() {
    GPIO_registers_type *PC = (GPIO_registers_type *)GPIOC_BASE;
    int ret = 0;

    ret = (read32(&PC->IDR) >> 13) & 1;

    // inversed, if LOW btn is pressed
    return ret == 0 ? 1 : 0;
}

static void set_led_state(int on) {
    GPIO_registers_type *PE = (GPIO_registers_type *)GPIOE_BASE;
    if (on) {
        // PE[2] = LOW led is on
        write16(&PE->BSRRH, (1 << 2));
    } else {
        // PE[2] = HIGH led is off
        write16(&PE->BSRRL, (1 << 2));
    }
}

portTASK_FUNCTION(task500ms, args)
{
    task500ms_data_type *task_data = (task500ms_data_type *)args;
    const TickType_t delay_ms = 500 / portTICK_PERIOD_MS;
    int btn_state_z = 0;
    int btn_state = 0;

    while (1) {
        // do something
        btn_state = is_button_pressed();
        if (btn_state && !btn_state_z) {
            task_data->service_mode = !task_data->service_mode;
        } else if (btn_state && !btn_state_z) {
            // negedge
        }
        btn_state_z = btn_state;

        if (task_data->service_mode) {
            // LED blinking in service mode:
            set_led_state(task_data->cnt & 1);
        } else {
            set_led_state(1);
        }
  
        vTaskDelay(delay_ms);
        task_data->cnt++;
    }
}

