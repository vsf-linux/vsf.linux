# please remove vsf/source/shell/sys/linux/include/simple_libc/cmath

cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../../../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations
# arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
# SDL need -fno-short-enums, or static assert failure in c++
set(CMAKE_C_FLAGS 
    "-Ofast -fno-short-enums"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS 
    "-Ofast -fno-short-enums"
    CACHE INTERNAL "C++ compiler common flags"
)
include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)

project(vsf_linux C CXX ASM)
set(VSF_TARGET STM32H743XI)

# VSF_USE configurations
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
set(VSF_USE_LWIP 0)
set(VSF_USE_BTSTACK 0)
    set(VSF_BTSTACK_USE_LINKKEY_MEMORY 0)
set(APP_USE_SDL_DEMO 1)
    set(APP_USE_SDL_SDLPAL_DEMO 1)

# include vsf cmake script
set(VSF_CONFIG_PATH ../../../config)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# application configurations
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    __VSF__
    __OOC_CPP__
# stm32h7xx.h need this
    STM32H757xx
    CORE_CM7
    DATA_IN_D2_SRAM
)
if(APP_USE_SDL_DEMO)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        VSF_SDL2_CFG_COLOR=VSF_DISP_COLOR_RGB565
    )
endif()

# linker script
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    LINKER:-Map=output.map
)

# add user application source code/include directories
add_subdirectory(../../../application ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_app)
add_subdirectory($ENV{VSF_PATH}/example/template/demo/common ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_app/common)
add_subdirectory($ENV{VSF_PATH}/example/template/demo/board/embedfire_stm32h743_pro ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}_app/board)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    $ENV{VSF_PATH}/example/template/demo/common
    $ENV{VSF_PATH}/example/template/demo/board/embedfire_stm32h743_pro
    $ENV{VSF_PATH}/example/template/demo/board/embedfire_stm32h743_pro/STM32H7xx_HAL_Driver/Inc
    $ENV{VSF_PATH}/source/hal/driver/ST/STM32H7/common/vendor/Include
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    $ENV{VSF_PATH}/example/template/demo/linux_demo/mount_demo.c
)

# type conversion and show size
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
)