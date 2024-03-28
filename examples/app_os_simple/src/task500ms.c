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
    if (data->user_btn.event & BTN_EVENT_PRESSED) {
        if (data->service_state < SERVICE_STATE_END) {
            data->service_state++;
            data->wait_btn = 0;
        }
        data->user_btn.event = 0;
    }

    // LED blinking in service mode:
    if (data->service_state == SERVICE_STATE_IDLE) {
        set_led_state(1);
        data->service_start_time = data->cnt;
        data->pause_cnt = 0;
        return;
    } else {
        set_led_state((int)(data->cnt & 1));
    }

    if (data->wait_btn) {
        return;
    }

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
        uart_printf("[%d] Start service demo\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_RELAY0_ENA;
        break;
    case SERVICE_STATE_RELAY0_ENA:
        relay_on(0);
        uart_printf("[%d] Relay[0] is on\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_RELAY1_ENA:
        relay_on(1);
        uart_printf("[%d] Relay[1] is on\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_RELAYS_DIS:
        relay_off(0);
        relay_off(1);
        uart_printf("[%d] Relay[0] and Relay[1] are off\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_LED_ON;
        break;
    case SERVICE_STATE_LED_ON:
        led_init(&data->led_data);
        led_on(&data->led_data);
        uart_printf("[%d] LED Line[0] turn on\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LED_OFF:
        led_off(&data->led_data);
        uart_printf("[%d] LED Line[0] turn off\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_SCALES_INIT;
        break;
    case SERVICE_STATE_SCALES_INIT:
        uart_printf("[%d] Init scales\r\n", xTaskGetTickCount());
        load_sensor_init(&data->load_sensor_data);
        data->service_state = SERVICE_STATE_SCALES_READ;
        break;
    case SERVICE_STATE_SCALES_READ:
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_END:
        uart_printf("[%d] End of service\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_IDLE;
        break;
    default:;
    }
}


portTASK_FUNCTION(task500ms, args)
{
    task500ms_data_type *task_data = (task500ms_data_type *)args;
    const TickType_t delay_ms = 500 / portTICK_PERIOD_MS;

    while (1) {
        // do something
        update_service_state(task_data);
 
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        task_data->cnt++;
    }
}

