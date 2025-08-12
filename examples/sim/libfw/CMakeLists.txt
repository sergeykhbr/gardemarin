cmake_minimum_required(VERSION 3.4.0)
project(libfw DESCRIPTION "firmware prototype library")

if(UNIX)
	set(LIBRARY_OUTPUT_PATH "../../linuxbuild/bin")
else()
	add_definitions(-D_UNICODE)
	add_definitions(-DUNICODE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(CMAKE_BUILD_TYPE Debug)

#set(_fw_root "./../../app_os_simple")
set(_fw_root "./../../canmonitor")

include_directories(
    ${_fw_root}/../common/include
    ${_fw_root}/../common/include/devs
    ${_fw_root}/../common/system
    ${_fw_root}/../common/fw
    ${_fw_root}/../common/drivers
    ${_fw_root}/../freertos/include
    ${_fw_root}/../freertos
    ${_fw_root}/src/system
    ./freertos
)

file(
    GLOB_RECURSE libfw_src 
    LIST_DIRECTORIES false
    "${_fw_root}/../common/*.c*"
    "${_fw_root}/../common/*.h"
    "${_fw_root}/src/*.c*"
    "${_fw_root}/src/*.h"
    "${_fw_root}/../freertos/event_groups.c"
    "${_fw_root}/../freertos/list.c"
    "${_fw_root}/../freertos/queue.c"
    "${_fw_root}/../freertos/stream_buffer.c"
    "${_fw_root}/../freertos/tasks.c"
    "${_fw_root}/../freertos/timers.c"
    "${_fw_root}/../freertos/MemMang/heap_4.c"
    "./freertos/*.c"
    "./freertos/*.h"
)

foreach(_source IN ITEMS ${libfw_src})
    get_filename_component(_source_path "${_source}" PATH)
    # form proper filter name from the path to source file:
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "${_fw_root}" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()


add_library(libfw STATIC
	${libfw_src}
)
target_link_libraries(libfw libst32sim)

