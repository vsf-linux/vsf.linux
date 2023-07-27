cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0076 NEW)

# set VSF_PATH and VSF_SRC_PATH
if(NOT DEFINED ENV{VSF_PATH})
    get_filename_component(__VSF_PATH ../vsf ABSOLUTE)
    set(ENV{VSF_PATH} ${__VSF_PATH})
endif()
message(STATUS "VSF_PATH: $ENV{VSF_PATH}")

# compiler configurations

if(TRUE)
  # arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
  # **************** Seems not Working ****************
  # refer to arm-none-eabi-gcc BUG: https://answers.launchpad.net/gcc-arm-embedded/+question/675869
  set(CMAKE_C_FLAGS 
      "-Og -fms-extensions -nostdlib -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4"
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS 
      "-Og -fms-extensions -nostdlib -nostartfiles -e _start -msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative -fPIC -shared -z max-page-size=4"
      CACHE INTERNAL "C++ compiler common flags"
  )
  include($ENV{VSF_PATH}/script/cmake/compilers/gnuarmemb.cmake)
else()
  if(NOT DEFINED LLVM_TOOLCHAIN_PATH)
    message(WARNING "LLVM_TOOLCHAIN_PATH not defined, use default: -DLLVM_TOOLCHAIN_PATH=\"E:/Software/armllvm16\"")
    set(LLVM_TOOLCHAIN_PATH "E:/Software/armllvm16")
  endif()
  if(NOT DEFINED LLVM_TOOLCHAIN_EXE_SUFIX)
    message(WARNING "LLVM_TOOLCHAIN_EXE_SUFIX not defined, use default: -DLLVM_TOOLCHAIN_EXE_SUFIX=\".exe\"")
    set(LLVM_TOOLCHAIN_EXE_SUFIX ".exe")
  endif()
  if(NOT DEFINED LLVM_TOOLCHAIN_SYSROOT)
    message(WARNING "LLVM_TOOLCHAIN_SYSROOT not defined, use default: -DLLVM_TOOLCHAIN_EXE_SUFIX=\"${LLVM_TOOLCHAIN_PATH}/lib/clang-runtimes/arm-none-eabi/armv7m_soft_nofp\"")
    set(LLVM_TOOLCHAIN_SYSROOT "${LLVM_TOOLCHAIN_PATH}/lib/clang-runtimes/arm-none-eabi/armv7m_soft_nofp")
  endif()

  set(CMAKE_C_FLAGS
#      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"      # GOT-base position independency
      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"                              # embedded position independency
      CACHE INTERNAL "C compiler common flags"
  )
  set(CMAKE_CXX_FLAGS
#      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"
      "-Oz -mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"
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
    __VSF_CPP__     # cpp support in PLOOC
    __CPU_ARM__     # needed in vsf_usr_cfg.h
)
vsf_add_include_directories(
    .
    ./include
    # linux include directories
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/inttypes
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/math
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/stdint
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc/assert
    $ENV{VSF_PATH}/source/shell/sys/linux/include/libusb
)
vsf_add_sources(
    main.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/glibc/vsf_linux_glibc_ctype.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/3rd-party/getopt/getopt_long.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/3rd-party/regex/regcomp.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/3rd-party/regex/regerror.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/3rd-party/regex/regexec.c
    $ENV{VSF_PATH}/source/shell/sys/linux/lib/3rd-party/regex/regfree.c
)
#vsf_add_libraries(
#)
