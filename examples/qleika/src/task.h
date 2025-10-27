/*
 *  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include <mcu.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum estate_type {
    State_SplashScreen,
    State_SelfTest,
    State_Wait,
    State_Idle
} estate_type;


#define BTN_Up     0x1
#define BTN_Center 0x2
#define BTN_Down   0x4


typedef struct raw_meas_type {
    char water_level;
    char water_low;     // no water detected during watering
    char watering_ena;
    uint8_t btn_event;
    uint16_t lux;
    int lux_error;
    int pressure;
    int pressure_error;
    int dht_error;
    int temperature;
    int moisture;
    int air_1p0;
    int air_2p5;
    int air_10;
} raw_meas_type;

typedef struct task_data_type {
    estate_type state;
    estate_type state_next;
    int wait_cnt;
    int state_changed_sec;
    raw_meas_type raw;
    int watering_cnt;  // 1800 seconds wait + 20 seconds watering
    char watering_mode;  // stored in BKP setting: wait/watering cycle duration index
    int watering_mode_sec; // last time it was changed
    char watering_redraw;  // redraw watering line after button events

    char tstr[21];     // 20 symbols of font24 per  line
} task_data_type;

void task_init(task_data_type *data);
void task_update(task_data_type *data, int sec);

#ifdef __cplusplus
}
#endif
