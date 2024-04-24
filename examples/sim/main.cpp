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

#include <sim_api.h>
#include <simutils.h>

extern "C" void SystemInit();
extern "C" int fwmain();
extern "C" void SysTick_Handler();
extern "C" void ADC1_irq_handler();

uint32_t __stdcall fw_thread(void *) {
    fwmain();
    return 0;
}

int main(int argc, char *argv[]) {
    int param;
    sim_init();

    // Enable VXL driver usage
    param = 0;
    sim_set_opt(SIM_OPT_VXL_DRIVER, &param);

    sim_register_isr(-1, SysTick_Handler);
    sim_register_isr(18, ADC1_irq_handler);
    sim_run_firmware(fw_thread);

    while (1) {
        Sleep(500);
    }
    sim_destroy();
    return 0;
}
