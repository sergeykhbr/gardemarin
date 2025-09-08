cmake_minimum_required(VERSION 3.2.0)
project (boot DESCRIPTION "Boot example for grademarin ST32F407 board")

#include(armgcc.cmake)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_BUILD_TYPE Debug)

# The following two flags need to remove "-rdynamic" flag that raises error on Windows
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_definitions(-D_ARMGCC)
add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)

set(FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard")
set(LD_FLAGS "${FLAGS} -nostartfiles")
set(LD_FLAGS "${FLAGS} -T ${CMAKE_CURRENT_SOURCE_DIR}/../common/system/stm32f4xx.ld")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS} -fno-use-cxa-atexit" )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LD_FLAGS} -fno-use-cxa-atexit")


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include/devs
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/system
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(
    GLOB_RECURSE _boot_src
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../common/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"

)

add_executable(boot ${_boot_src})

# Generate *.bin file
add_custom_command(TARGET boot POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -v -O binary 
                             "${CMAKE_CURRENT_BINARY_DIR}/boot"
                             "${CMAKE_CURRENT_BINARY_DIR}/boot.bin"
)

