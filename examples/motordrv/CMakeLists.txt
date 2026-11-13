cmake_minimum_required(VERSION 3.20)
project(motordrv LANGUAGES C CXX ASM)

set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(SOURCE_DIRS
    "${COMMON_DIR}/system"
    "${COMMON_DIR}/drivers"
    "${COMMON_DIR}/fw"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/system"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_executable(motordrv)

target_sources(motordrv PRIVATE
    stm32f4xx_isr_vector
    system_handlers
    system_init
    gcc_startup
    gcc_newlib
    vprintfmt
    uart
    app_handlers
    event_groups
    list
    queue
    stream_buffer
    tasks
    timers
    port
    heap_4
    app_hooks
    gpio_drv
    user_btn
    user_led
    uart_drv
    adc_drv
    relais
    loadsensor
    ledstrip
    hbridge_dcmotor
    hbridge_current
    hbridge
    can_drv
    ds18b20_drv
    soil_drv
    rtc_drv
    usrsettings
    fwmem
    fwapi
    fwattribute
    fwobject
    fwkernelgen
    fwfifo
    dbc
    app_fwkernel
    ManagementClass
    task1ms
    taskEpoch
    fwmain
)

target_include_directories(motordrv PRIVATE
    "${COMMON_DIR}/include"
    "${COMMON_DIR}/include/devs"
    "${COMMON_DIR}/system"
    "${COMMON_DIR}/drivers"
    "${COMMON_DIR}/fw"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/system"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

set_target_properties(motordrv PROPERTIES
    OUTPUT_NAME "motordrv"
    SUFFIX ".elf"
)

set(ARM_CPU_FLAGS
    -mcpu=cortex-m4
    -mfloat-abi=hard
)

set(COMMON_COMPILE_FLAGS
    -g
    -Os
    -static
    -fno-common
    -DxSHOW_DBC
)

target_compile_options(motordrv PRIVATE
    ${ARM_CPU_FLAGS}
    ${COMMON_COMPILE_FLAGS}
    $<$<COMPILE_LANGUAGE:C>:-std=gnu99>
    $<$<COMPILE_LANGUAGE:ASM>:-D__ASSEMBLY__=1>
)

target_link_options(motordrv PRIVATE
    ${ARM_CPU_FLAGS}
    -nostartfiles
    "-T${COMMON_DIR}/system/stm32f4xx.ld"
)

target_link_libraries(motordrv PRIVATE
    gcc
    stdc++
    c
    m
)

# Post compilation:
add_custom_command(TARGET motordrv POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        "-DELF_FILE=$<TARGET_FILE:motordrv>"
        "-DOUTPUT_DIR=${CMAKE_BINARY_DIR}"
        "-DOBJDUMP=${CMAKE_OBJDUMP}"
        "-DOBJCOPY=${CMAKE_OBJCOPY}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/GenerateArtifacts.cmake"
    COMMENT "Generating motordrv.dump, motordrv.lst and motordrv.bin"
    VERBATIM
)

message(STATUS "--------------------------------------")
message(STATUS "Motor Driver configuration")
message(STATUS "--------------------------------------")
message(STATUS "Build type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "Linker script: ${LINKER_SCRIPT}")
message(STATUS "Output:        ${CMAKE_BINARY_DIR}")
message(STATUS "--------------------------------------")
