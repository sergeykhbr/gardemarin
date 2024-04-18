cmake_minimum_required(VERSION 3.4.0)
project(libst32sim DESCRIPTION "ST32F407* simplified CPU simulator library")

if(UNIX)
	set(LIBRARY_OUTPUT_PATH "../../linuxbuild/bin")
else()
	add_definitions(-D_UNICODE)
	add_definitions(-DUNICODE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(CMAKE_BUILD_TYPE Debug)

set(_st32_root ".")
set(_simcommon "../simcommon")

include_directories(
    ${_st32_root}/../../common/include
    ${_st32_root}/../../common/include/devs
    ${_simcommon}
    ${_st32_root}/src
    ${_st32_root}/src/generic
    ${_st32_root}/src/utils
)

file(
    GLOB_RECURSE libst32sim_src 
    LIST_DIRECTORIES false
    "${_st32_root}/*.c*"
    "${_st32_root}/*.h"
)

foreach(_source IN ITEMS ${libst32sim_src})
    get_filename_component(_source_path "${_source}" PATH)
    # form proper filter name from the path to source file:
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "${_st32_root}" "" _group_path "${_group_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

file(GLOB simcommon_src ${_simcommon}/*.cpp ${_simcommon}/*.h)
source_group("simcommon"  FILES ${simcommon_src})


add_library(libst32sim STATIC
	${libst32sim_src}
	${simcommon_src}
)

