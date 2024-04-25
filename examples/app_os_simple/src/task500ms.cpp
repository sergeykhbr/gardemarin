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
#include <BinInterface.h>
#include <CanInterface.h>
#include <TimerInterface.h>
#include <RunInterface.h>
#include <PwmInterface.h>
#include "app_tasks.h"

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

void output_scales() {
    FwObject *obj = reinterpret_cast<FwObject *>(fw_get_object("scales"));
    FwAttribute *value0 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "value0"));
    FwAttribute *value1 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "value1"));
    FwAttribute *value2 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "value2"));
    FwAttribute *value3 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "value3"));

    FwAttribute *gram0 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "gram0"));
    FwAttribute *gram1 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "gram1"));
    FwAttribute *gram2 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "gram2"));
    FwAttribute *gram3 = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "gram3"));

    uart_printf("0 %08x %d  1 %08x %d  2 %08x %d  3 %08x %d\r\n",
                value0->to_uint32(),
                static_cast<int32_t>(gram0->to_float()),
                value1->to_uint32(),
                static_cast<int32_t>(gram1->to_float()),
                value2->to_uint32(),
                static_cast<int32_t>(gram2->to_float()),
                value3->to_uint32(),
                static_cast<int32_t>(gram3->to_float()));
}

void output_adc() {
    FwObject *obj = reinterpret_cast<FwObject *>(fw_get_object("adc1"));
    FwAttribute *temp = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "temperature"));
    FwAttribute *vref = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "Vref"));
    FwAttribute *vbat = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "Vbat"));

    uart_printf("adc1 T=%d VREFINT=%d VBAT=%d\r\n",
                temp->to_int32(),
                vref->to_int32(),
                vbat->to_int32());

    obj = reinterpret_cast<FwObject *>(fw_get_object("hbrg0"));
    FwAttribute *i0_value = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "i0_value"));
    FwAttribute *i1_value = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "i1_value"));

    uart_printf("hrg0 %d %d\r\n",
                i0_value->to_int32(),
                i1_value->to_int32());

    obj = reinterpret_cast<FwObject *>(fw_get_object("hbrg1"));
    i0_value = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "i0_value"));
    i1_value = reinterpret_cast<FwAttribute *>(
            fw_get_obj_attr_by_name(obj, "i1_value"));

    uart_printf("hrg1 %d %d\r\n",
                i0_value->to_int32(),
                i1_value->to_int32());
}


void write_obj_attribute(const char *objname,
                         const char *atrname,
                         char *buf,
                         int sz) {
    FwAttribute *atr = reinterpret_cast<FwAttribute *>(
                fw_get_object_attribute(objname, atrname));
    if (atr) {
        atr->write(buf, sz);
    } else {
        uart_printf("[%d] %s:%s not found\r\n",
            xTaskGetTickCount(), objname, atrname);
    }
}

void relais_on(const char *name) {
    CommonInterface *iface = reinterpret_cast<CommonInterface *>(
                fw_get_object_interface(name, "BinInterface"));
    if (iface) {
        static_cast<BinInterface *>(iface)->setBinEnabled();
        uart_printf("[%d] %s is on\r\n", xTaskGetTickCount(), name);
    } else {
        uart_printf("[%d] %s interface not found\r\n", xTaskGetTickCount(), name);
    }
}

void relais_off(const char *name) {
    CommonInterface *iface = reinterpret_cast<CommonInterface *>(
                fw_get_object_interface(name, "BinInterface"));
    if (iface) {
        static_cast<BinInterface *>(iface)->setBinDisabled();
        uart_printf("[%d] %s is off\r\n", xTaskGetTickCount(), name);
    } else {
        uart_printf("[%d] %s interface not found\r\n", xTaskGetTickCount(), name);
    }
}

void led_strip_on(const char *name) {
    CommonInterface *iface = reinterpret_cast<CommonInterface *>(
          fw_get_object_port_interface("ledrbw", name, "PwmInterface"));
    if (iface) {
        static_cast<PwmInterface *>(iface)->enablePwm();
        uart_printf("[%d] LED %s turn on, dim=100\r\n", xTaskGetTickCount(), name);
    } else {
        uart_printf("[%d] LED PWM %s not found\r\n", xTaskGetTickCount(), name);
    }
}

void led_strip_off(const char *name) {
    CommonInterface *iface = reinterpret_cast<CommonInterface *>(
          fw_get_object_port_interface("ledrbw", name, "PwmInterface"));
    if (iface) {
        static_cast<PwmInterface *>(iface)->disablePwm();
        uart_printf("[%d] LED %s turn off\r\n", xTaskGetTickCount(), name);
    } else {
        uart_printf("[%d] LED PWM %s not found\r\n", xTaskGetTickCount(), name);
    }
}

void dc_motor_on(int idx) {
    CommonInterface *iface;
    char hbrg[8] = "hbrg0";
    char dc[4] = "dc0";
    hbrg[4] += (idx >> 1);
    dc[2] += (idx & 1);

    iface = reinterpret_cast<CommonInterface *>(
           fw_get_object_port_interface(hbrg, dc, "PwmInterface"));
    if (iface) {
        static_cast<PwmInterface *>(iface)->enablePwm();
        uart_printf("[%d] dc[%d] started\r\n", xTaskGetTickCount(), idx);
    } else {
        uart_printf("[%d] dc[%d] PwmInterface not found\r\n", xTaskGetTickCount(), idx);
    }
}

void dc_motor_off(int idx) {
    CommonInterface *iface;
    char hbrg[8] = "hbrg0";
    char dc[4] = "dc0";
    hbrg[4] += (idx >> 1);
    dc[2] += (idx & 1);

    iface = reinterpret_cast<CommonInterface *>(
           fw_get_object_port_interface(hbrg, dc, "PwmInterface"));
    if (iface) {
        static_cast<PwmInterface *>(iface)->disablePwm();
        uart_printf("[%d] dc[%d] stopped\r\n", xTaskGetTickCount(), idx);
    } else {
        uart_printf("[%d] dc[%d] PwmInterface not found\r\n", xTaskGetTickCount(), idx);
    }
}

void update_service_state(app_data_type *data) {
    // LED blinking in service mode:
    CommonInterface *iface;
    int btnClick = data->keyNotifier->btnClick;
    char rawbuf[4];
    data->keyNotifier->btnClick = 0;

    // Expected current through the Relay at 5V is 89.3 mA
    // Relay[0]=off; Relay[1]=off; I=60 mA
    // Relay[0]=off; Relay[1]=on;  I=150 mA
    // Relay[0]=on;  Relay[1]=off; I=150 mA
    // Relay[0]=on;  Relay[1]=on;  I=230 mA
   
    switch (data->service_state) {
    case SERVICE_STATE_IDLE:
        data->service_start_time = data->cnt;
        if (btnClick) {
            rawbuf[0] = 2;      // Blink mode
            write_obj_attribute("uled0", "state", rawbuf, 1);
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
    case SERVICE_STATE_SCALES_READ:
        output_scales();
        if (btnClick) {
            data->service_state++;
        }
        break;
    case SERVICE_STATE_SCALES_SLEEP:
        iface = reinterpret_cast<CommonInterface *>(
                  fw_get_object_interface("scales", "RunInterface"));
        if (iface) {
            static_cast<RunInterface *>(iface)->setSleep();
            uart_printf("[%d] Scales turned off\r\n", xTaskGetTickCount());
        } else {
            uart_printf("[%d] Scales RunInterface not found\r\n", xTaskGetTickCount());
        }
        data->service_state++;
        break;
    case SERVICE_ADC_CHECK:
        output_adc();
        if (btnClick) {
            data->service_state++;
        }
        break;
    case SERVICE_STATE_END:
        uart_printf("[%d] End of service\r\n", xTaskGetTickCount());
        rawbuf[0] = 1;      // On mode
        write_obj_attribute("uled0", "state", rawbuf, 1);
        data->service_state = SERVICE_STATE_IDLE;
        break;
    default:;
    }
}

void KeyNotifierType::keyPressed() {
    xTaskNotify(reinterpret_cast<app_data_type *>(data)->handleTask500ms,
                0,
                eNoAction);
    btnClick = true;
}

void KeyNotifierType::waitKeyPressed() {
    uint32_t notifiedValue=0;
    xTaskNotifyStateClear(
        reinterpret_cast<app_data_type *>(data)->handleTask500ms);
    xTaskNotifyWait(0x00,   // don't clear any notification bits on entry
                    0xffffffffUL,  // Reset the notification value to 0 on exit
                    &notifiedValue,
                    portMAX_DELAY); // Block indefinetly
}

portTASK_FUNCTION(task500ms, args)
{
    app_data_type *task_data = (app_data_type *)args;
    const TickType_t delay_ms = 500 / portTICK_PERIOD_MS;

    KeyInterface *iface = reinterpret_cast<KeyInterface *>(
            fw_get_object_interface("ubtn0", "KeyInterface"));
    if (iface) {
        iface->registerKeyListener(
            static_cast<KeyListenerInterface *>(task_data->keyNotifier));
    }

    while (1) {
        // do something
        update_service_state(task_data);

        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

