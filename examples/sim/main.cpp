#include <s32k148api.h>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>

extern "C" void SystemInit();
extern "C" int main_fw();
extern "C" void LPIT0_Ch0_ISR();
extern "C" void PORTC_ISR();
extern "C" void CAN0_MSG_0_15_ISR();

uint32_t __stdcall fw_thread(void *) {
    main_fw();
    return 0;
}

int main(int argc, char *argv[]) {
    int param;
    s32k148_init();

    // Enable VXL driver usage
    param = 0;
    s32k148_set_opt(S32K148_OPT_VXL_DRIVER, &param);

    s32k148_register_isr(48, LPIT0_Ch0_ISR);
    s32k148_register_isr(61, PORTC_ISR);
    s32k148_register_isr(81, CAN0_MSG_0_15_ISR);
    s32k148_run_firmware(fw_thread);

    while (1) {
        Sleep(500);
    }
    s32k148_destroy();
    return 0;
}
