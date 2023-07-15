cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

# set VSF_PATH and VSF_SRC_PATH
if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations

if(NOT TRUE)
  # arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
  # **************** Seems not Working ****************
  # refer to arm-none-eabi-gcc BUG: https://answers.launchpad.net/gcc-arm-embedded/+question/675869
  set(CMAKE_C_FLAGS 
      "-Ofast -fms-extensions -nostdlib -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4"
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS 
      "-Ofast -fms-extensions -nostdlib -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4"
      CACHE INTERNAL "C++ compiler common flags"
  )
  include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)
else()
  set(LLVM_TOOLCHAIN_PATH "E:/Software/armllvm16")
  set(LLVM_TOOLCHAIN_EXE_SUFIX ".exe")
  set(CMAKE_EXE_LINKER_FLAGS "--sysroot ${LLVM_TOOLCHAIN_PATH}/lib/clang-runtimes/armv7m_soft_nofp -nostdlib")
  set(CMAKE_C_FLAGS 
      "-Oz --target=armv7m-none-eabi -mcpu=cortex-m4 -mthumb -fropi -frwpi -fno-builtin-printf -z max-page-size=4"
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS 
      "-Oz --target=armv7m-none-eabi -mcpu=cortex-m4 -mthumb -fropi -frwpi -fno-builtin-printf -z max-page-size=4"
      CACHE INTERNAL "C++ compiler common flags"
  )
  include($ENV{VSF_PATH}/script/cmake/compilers/armllvm.cmake)
endif()

project(vsf_applet C CXX ASM)

# target & configurations
set(VSF_TARGET CortexM4)
set(VSF_APPLET 1)
set(VSF_USE_LINUX 1)
    set(VSF_LINUX_USE_SIMPLE_LIBC 1)
include($ENV{VSF_PATH}/script/cmake/vsf.cmake)

# linker
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    LINKER:-T${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    LINKER:-Map=output.map
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    SUFFIX ".elf"
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)

# add user application source code/include directories
vsf_add_compile_definitions(
    __OOC_CPP__     # cpp support in PLOOC
    __CPU_ARM__     # needed in vsf_usr_cfg.h
)
vsf_add_include_directories(
    .
    ./include
    # linux include directories
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
    $ENV{VSF_PATH}/source/shell/sys/linux/include/libusb
)
vsf_add_sources(
    main.c
)
