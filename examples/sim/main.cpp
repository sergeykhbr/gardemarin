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

#include <s32k148api.h>
#include <simutils.h>

extern "C" void SystemInit();
extern "C" int fwmain();
//extern "C" void LPIT0_Ch0_ISR();
//extern "C" void PORTC_ISR();
//extern "C" void CAN0_MSG_0_15_ISR();

uint32_t __stdcall fw_thread(void *) {
    fwmain();
    return 0;
}

int main(int argc, char *argv[]) {
    int param;
    s32k148_init();

    // Enable VXL driver usage
    param = 0;
    s32k148_set_opt(S32K148_OPT_VXL_DRIVER, &param);

//    s32k148_register_isr(48, LPIT0_Ch0_ISR);
//    s32k148_register_isr(61, PORTC_ISR);
//    s32k148_register_isr(81, CAN0_MSG_0_15_ISR);
    s32k148_run_firmware(fw_thread);

    while (1) {
        Sleep(500);
    }
    s32k148_destroy();
    return 0;
}
