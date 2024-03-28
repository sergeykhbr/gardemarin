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
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <btn_usr.h>
#include <loadsensor.h>
#include <leddrv.h>

#pragma once

#define APP_TASK_NAME "app"

#define SERVICE_STATE_IDLE        0
#define SERVICE_STATE_INIT        1
#define SERVICE_STATE_RELAY0_ENA  2 
#define SERVICE_STATE_RELAY1_ENA  3
#define SERVICE_STATE_RELAYS_DIS  4
#define SERVICE_STATE_LED_ON      5
#define SERVICE_STATE_LED_OFF     6
#define SERVICE_STATE_SCALES_INIT 7
#define SERVICE_STATE_SCALES_READ 8
#define SERVICE_STATE_END         9

typedef struct task500ms_data_type {
    uint64_t cnt;
    uint64_t service_start_time;
    int service_state;
    int pause_cnt;
    int wait_btn;

    btn_user_type user_btn;
    load_sensor_type load_sensor_data;
    led_channel_type led_data;
} task500ms_data_type;

portTASK_FUNCTION(task500ms, args);
