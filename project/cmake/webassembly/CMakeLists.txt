cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED ENV{VSF_PATH})
    message(FATAL_ERROR "Please define environment VSF_PATH")
else()
    message(STATUS "VSF_PATH: $ENV{VSF_PATH}")
endif()

# compiler configurations
# if compiler is not set, cmake will find one
if(NOT DEFINED ENV{EMSDK})
    message(FATAL_ERROR "Please install emsdk first")
else()
    message(STATUS "EMSDK: $ENV{EMSDK}")
endif()
include($ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)

project(vsf_template)
set(VSF_TARGET webassembly/emscripten)

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
get_filename_component(VSF_LVGL_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/lvgl ABSOLUTE)
get_filename_component(VSF_EVM_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/evm ABSOLUTE)
get_filename_component(VSF_MBEDTLS_CONFIG_PATH ${VSF_CONFIG_PATH}/3rd-party/mbedtls ABSOLUTE)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# add user application source code/include directories
# add template directory
add_subdirectory($ENV{VSF_PATH}/example/template/demo/common ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_app/common)
add_subdirectory(../../../application ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_app)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    $ENV{VSF_PATH}/example/template/demo/common
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    $ENV{VSF_PATH}/example/template/demo/linux_demo/libusb_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/mount_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/ntpdate_demo.c
    $ENV{VSF_PATH}/example/template/demo/linux_demo/telnetd_demo.c
)