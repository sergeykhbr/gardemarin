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
#pragma once

#include <prjtypes.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <user_btn.h>
#include <user_led.h>
#include <relais.h>
#include <loadsensor.h>
#include <ledstrip.h>
#include <motor.h>

#define APP_TASK_NAME "app"

#define SERVICE_STATE_IDLE        0
#define SERVICE_STATE_INIT        1
#define SERVICE_STATE_RELAY0_ENA  2 
#define SERVICE_STATE_RELAY1_ENA  3
#define SERVICE_STATE_RELAYS_DIS  4
#define SERVICE_STATE_LED0_ON     5
#define SERVICE_STATE_LED1_ON     6
#define SERVICE_STATE_LED2_ON     7
#define SERVICE_STATE_LED3_ON     8
#define SERVICE_STATE_LEDALL_ON   9
#define SERVICE_STATE_LEDALL_OFF  10
#define SERVICE_STATE_SCALES_INIT 11
#define SERVICE_STATE_SCALES_READ 12
#define SERVICE_STATE_MOTOR0_ENA  13
#define SERVICE_STATE_MOTOR1_ENA  14
#define SERVICE_STATE_MOTOR_DIS   15
#define SERVICE_STATE_END         16

typedef struct task500ms_data_type {
    uint64_t cnt;
    uint64_t service_start_time;
    int service_state;
    int pause_cnt;
    int wait_btn;

    user_btn_type user_btn;
    user_led_type user_led;
    relais_type relais_data;
    load_sensor_type load_sensor_data;
    led_strip_type led_data;
    motor_driver_type motor_data;
} task500ms_data_type;

portTASK_FUNCTION(task500ms, args);
