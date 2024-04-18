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
#include <KeyInterface.h>

#define APP_TASK_NAME "app"

#define SERVICE_STATE_IDLE         0
#define SERVICE_STATE_INIT         1
#define SERVICE_STATE_CAN1_START   2 
#define SERVICE_STATE_CAN1_SNIFFER 3 
#define SERVICE_STATE_CAN1_STOP    4
#define SERVICE_STATE_RELAY        5 
#define SERVICE_STATE_LED          6
#define SERVICE_STATE_SCALES_READ  7
#define SERVICE_STATE_SCALES_SLEEP 8
#define SERVICE_STATE_MOTOR        9
#define SERVICE_STATE_END          10

class KeyNotifierType : public KeyListenerInterface {
 public:
    virtual void keyPressed() override;
    virtual void keyReleased() override {}
    virtual void keyClick() override {}
    virtual void keyDoubleClick() override {}
    virtual void keyLongClick() override {}

    void waitKeyPressed();

    void *data;
    bool btnClick;
};

typedef struct app_data_type {
    uint64_t cnt;
    uint64_t service_start_time;
    int service_state;

    TaskHandle_t handleTask1ms;
    TaskHandle_t handleTask500ms;
    KeyNotifierType *keyNotifier;
} app_data_type;


portTASK_FUNCTION(task1ms, args);
portTASK_FUNCTION(task500ms, args);
