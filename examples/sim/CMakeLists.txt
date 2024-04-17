cmake_minimum_required(VERSION 3.4.0)
project(appsim)

if(UNIX)
	set(EXECUTABLE_OUTPUT_PATH "linuxbuild/bin")
	set(INSTALL_DIR "linuxbuild/bin")
	add_compile_options(-rdynamic)
else()
	set(CMAKE_INSTALL_PREFIX ".")
	add_definitions(-D_UNICODE)
	add_definitions(-DUNICODE)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_SUPPRESS_REGENERATION true)
endif()

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(libst32sim)
add_subdirectory(libfw)

include_directories(
    ../common/include
    simcommon
    libst32sim/src
)

add_executable(
  appsim
  main.cpp
)


if(UNIX)
	target_link_libraries(pthread rt dl)
else()
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT appsim)
	target_link_libraries(appsim libfw libst32sim)
endif()
 
