::C:\tools\OpenOCD-20250710-0.12.0\bin\openocd.exe -f stm32f1xx-boot.cfg
C:\soft\OpenOCD-20250710-0.12.0\bin\openocd.exe -f stm32f1xx-boot.cfg

:: start gdb:
::   target remote localhost:3333
::   add-symbol-file C:/Projects/gardemarin/examples/qleika/bin/qleika.elf
::   print variable_name
::    n   - step-over function
::    thbreak _malloc_r    - one time hw breakpoint
