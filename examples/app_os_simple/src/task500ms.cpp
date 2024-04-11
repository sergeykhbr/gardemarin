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
#include <BinInterface.h>
#include <CanInterface.h>
#include "task500ms.h"

// current task is 0.5 sec
#define SERVICE_SEC_TO_COUNT(sec) (2 * sec)

/** Print CAN frames
*/
void output_can_messages(can_frame_type *frame) {
    uart_printf("%d=>%08x %d",
                frame->busid,
                frame->id,
                frame->dlc);
    for (uint8_t n = 0; n < frame->dlc; n++) {
        uart_printf(" %02x", frame->data.u8[n]);
    }
     uart_printf("%s", "\r\n");
}


void update_service_state(task500ms_data_type *data) {
    // LED blinking in service mode:
    CommonInterface *iface;
    int btnClick = 0;
    if (data->user_btn.event & BTN_EVENT_PRESSED) {
        data->user_btn.event = 0;
        btnClick = 1;
    }

    if (data->service_state == SERVICE_STATE_IDLE) {
        user_led_set_state(1);
    } else {
        user_led_set_state((int)(data->cnt & 1));
    }

    if (data->wait_btn) {
        if (btnClick == 0) {
            return;
        } else {
            data->wait_btn = 0;
            data->service_state++;
        }
    }
    // Expected current through the Relay at 5V is 89.3 mA
    // Relay[0]=off; Relay[1]=off; I=60 mA
    // Relay[0]=off; Relay[1]=on;  I=150 mA
    // Relay[0]=on;  Relay[1]=off; I=150 mA
    // Relay[0]=on;  Relay[1]=on;  I=230 mA
    
    switch (data->service_state) {
    case SERVICE_STATE_IDLE:
        data->service_start_time = data->cnt;
        if (btnClick) {
            data->service_state++;
        }
        break;
    case SERVICE_STATE_INIT:
        uart_printf("[%d] Start service demo\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_CAN1_START:
        uart_printf("[%d] CAN1 sniffer started\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_CAN1_SNIFFER:
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("can1", "CanInterface"));
        if (iface) {
            can_frame_type can_data;
            while (static_cast<CanInterface *>(iface)->ReadCanFrame(&can_data)) {
                output_can_messages(&can_data);
            }
        }
        if (btnClick) {
            data->service_state++;
        }
        break;
    case SERVICE_STATE_CAN1_STOP:
        uart_printf("[%d] CAN1 stopped\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_RELAY0_ENA:
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("relais0", "BinInterface"));
        if (iface) {
            static_cast<BinInterface *>(iface)->setBinEnabled();
            uart_printf("[%d] relais0 is on\r\n", xTaskGetTickCount());
            data->wait_btn = 1;
        } else {
            uart_printf("[%d] relais0 interface not found\r\n", xTaskGetTickCount());
            data->service_state++;
        }
        break;
    case SERVICE_STATE_RELAY1_ENA:
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("relais1", "BinInterface"));
        if (iface) {
            static_cast<BinInterface *>(iface)->setBinEnabled();
            uart_printf("[%d] relais1 is on\r\n", xTaskGetTickCount());
            data->wait_btn = 1;
        } else {
            uart_printf("[%d] relais1 interface not found\r\n", xTaskGetTickCount());
            data->service_state++;
        }
        break;
    case SERVICE_STATE_RELAYS_DIS:
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("relais0", "BinInterface"));
        if (iface) {
            static_cast<BinInterface *>(iface)->setBinDisabled();
        }
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("relais1", "BinInterface"));
        if (iface) {
            static_cast<BinInterface *>(iface)->setBinDisabled();
        }
        uart_printf("[%d] Relais[0] and Relais[1] are off\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_LED0_ON:
        led_strip_on(0, 100);
        uart_printf("[%d] LED Strip[0] turn on, dim=100\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LED1_ON:
        led_strip_off(0);
        led_strip_on(1, 100);
        uart_printf("[%d] LED Strip[1] turn on, dim=100\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LED2_ON:
        led_strip_off(1);
        led_strip_on(2, 100);
        uart_printf("[%d] LED Strip[2] turn on, dim=100\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LED3_ON:
        led_strip_off(2);
        led_strip_on(3, 100);
        uart_printf("[%d] LED Strip[3] turn on, dim=100\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LEDALL_ON:
        led_strip_on(-1, 100);
        uart_printf("[%d] LED strips all on, dim=100\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_LEDALL_OFF:
        led_strip_off(-1);
        uart_printf("[%d] LED Strips off\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_SCALES_INIT:
        uart_printf("[%d] Init scales\r\n", xTaskGetTickCount());
        load_sensor_init(&data->load_sensor_data);
        data->service_state = SERVICE_STATE_SCALES_READ;
        break;
    case SERVICE_STATE_SCALES_READ:
        load_sensor_read(&data->load_sensor_data);
        if (btnClick) {
            data->service_state++;
        }
        break;
    case SERVICE_STATE_SCALES_SLEEP:
        load_sensor_sleep(&data->load_sensor_data);
        uart_printf("[%d] Scales turned off\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_MOTOR0_ENA:
        motor_dc_start(0);
        uart_printf("[%d] Pump[0] started\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_MOTOR1_ENA:
        motor_dc_stop(0);
        motor_dc_start(1);
        uart_printf("[%d] Pump[0] stopped; Pump[1] started\r\n", xTaskGetTickCount());
        data->wait_btn = 1;
        break;
    case SERVICE_STATE_MOTOR_DIS:
        motor_dc_stop(1);
        uart_printf("[%d] Pump[1] stopped\r\n", xTaskGetTickCount());
        data->service_state++;
        break;
    case SERVICE_STATE_END:
        uart_printf("[%d] End of service\r\n", xTaskGetTickCount());
        data->service_state = SERVICE_STATE_IDLE;
        break;
    default:;
    }
}

extern "C" portTASK_FUNCTION(task500ms, args)
{
    task500ms_data_type *task_data = (task500ms_data_type *)args;
    const TickType_t delay_ms = 500 / portTICK_PERIOD_MS;

    while (1) {
        // do something
        update_service_state(task_data);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        task_data->user_btn.tm_count = ++task_data->cnt;
    }
}

