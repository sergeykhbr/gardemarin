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

static void relay_on(int idx) {
    GPIO_registers_type *PD = (GPIO_registers_type *)GPIOD_BASE;
    // BSRR[15:0] Set bit (set to HIGH)
    if (idx == 0) {
        // PD[13] Relay0
        write16(&PD->BSRRL, (1 << 13));
    } else {
        // PD[14] Relay1
        write16(&PD->BSRRL, (1 << 14));
    }
}

static void relay_off(int idx) {
    GPIO_registers_type *PD = (GPIO_registers_type *)GPIOD_BASE;
    // BSRR[31:16] Reset bit (set to LOW)
    if (idx == 0) {
        // PD[13] Relay0
        write16(&PD->BSRRH, (1 << 13));
    } else {
        // PD[14] Relay1
        write16(&PD->BSRRH, (1 << 14));
    }
}

// current task is 0.5 sec
#define SERVICE_SEC_TO_COUNT(sec) (2 * sec)

void update_service_state(task500ms_data_type *data) {
    if (data->service_mode == 0) {
        set_led_state(1);
        data->service_start_time = data->cnt;
        data->service_state = SERVICE_STATE_INIT;
        data->pause_cnt = 0;
        return;
    }
    // LED blinking in service mode:
    set_led_state((int)(data->cnt & 1));

    if (data->pause_cnt) {
        data->pause_cnt--;
        return;
    }

    // Expected current through the Relay at 5V is 89.3 mA
    // Relay[0]=off; Relay[1]=off; I=60 mA
    // Relay[0]=off; Relay[1]=on;  I=150 mA
    // Relay[0]=on;  Relay[1]=off; I=150 mA
    // Relay[0]=on;  Relay[1]=on;  I=230 mA
    
    switch (data->service_state) {
    case SERVICE_STATE_INIT:
        data->service_state = SERVICE_STATE_RELAY0_ENA;
        uart_printf("[%d] Start service demo\r\n", xTaskGetTickCount());
        break;
    case SERVICE_STATE_RELAY0_ENA:
        relay_on(0);
        uart_printf("[%d] Relay[0] is on\r\n", xTaskGetTickCount());
        data->pause_cnt = SERVICE_SEC_TO_COUNT(2);
        data->service_state = SERVICE_STATE_RELAY1_ENA;
        break;
    case SERVICE_STATE_RELAY1_ENA:
        relay_on(1);
        uart_printf("[%d] Relay[1] is on\r\n", xTaskGetTickCount());
        data->pause_cnt = SERVICE_SEC_TO_COUNT(4);
        data->service_state = SERVICE_STATE_RELAYS_DIS;
        break;
    case SERVICE_STATE_RELAYS_DIS:
        relay_off(0);
        relay_off(1);
        uart_printf("[%d] Relay[0] and Relay[1] are off\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_READ_LOAD0;
        break;
    case SERVICE_STATE_READ_LOAD0:
        data->service_state = SERVICE_STATE_END;
        break;
    case SERVICE_STATE_END:
        uart_printf("[%d] End of service\r\n", xTaskGetTickCount());
        data->service_mode = 0;
        break;
    default:;
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
            uart_printf("User btn %s\r\n", "pressed");
        } else if (!btn_state && btn_state_z) {
            // negedge
            uart_printf("User btn %s\r\n", "released");
        }
        btn_state_z = btn_state;

        update_service_state(task_data);
 
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        task_data->cnt++;
    }
}

