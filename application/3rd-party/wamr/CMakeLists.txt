# CMakeLists head
# list(FILTER ) is introduced in 3.6
cmake_minimum_required(VERSION 3.6)

cmake_policy(SET CMP0079 NEW)

set(WAMR_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/raw)
set(WAMR_BUILD_TARGET THUMB)
set(WAMR_BUILD_PLATFORM linux)
set(WAMR_BUILD_INTERP 1)
#set(WAMR_BUILD_FAST_INTERP 1)
set(WAMR_BUILD_DEBUG_INTERP 1)
set(WAMR_BUILD_AOT 1)
set(WAMR_BUILD_JIT 0)
set(WAMR_BUILD_LIBC_BUILTIN 1)
set(WAMR_BUILD_LIBC_WASI 0)
set(WAMR_BUILD_MULTI_MODULE 0)
set(WAMR_BUILD_SHARED_MEMORY 0)
set(WAMR_BUILD_BULK_MEMORY 0)
set(WAMR_BUILD_THREAD_MGR 0)
set(WAMR_BUILD_LIB_PTHREAD 1)
set(WAMR_BUILD_SIMD 1)

include (${WAMR_ROOT_DIR}/build-scripts/runtime_lib.cmake)
# posix_memmap.c and gdbserver.c are replaced in port
set(WAMR_BUILD_SKIP_FILE_LIST
    core/shared/platform/common/posix/posix_memmap.c
    core/iwasm/libraries/debug-engine/gdbserver.c
)
foreach(skip_file ${WAMR_BUILD_SKIP_FILE_LIST})
    list(FILTER WAMR_RUNTIME_LIB_SOURCE EXCLUDE REGEX "${skip_file}$")
endforeach()

add_library(vmlib
    ${WAMR_RUNTIME_LIB_SOURCE}
    port/wamr_port.c
    port/product-mini/platforms/posix/main.c
    port/core/iwasm/libraries/debug-engine/gdbserver.c
    port/core/iwasm/libraries/libc-vsfwasi/libc_vsfwasi_wrapper.c
    port/core/shared/utils/uncommon/bh_read_file.c
)
target_include_directories(vmlib BEFORE PUBLIC
    ${VSF_COMMON_INCLUDE_DIRECTORIES}
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    port/core/shared/utils/uncommon
    port/core/shared/platform/vsf
)
if(VSF_LINUX_USE_SIMPLE_LIBC)
    target_include_directories(vmlib BEFORE PUBLIC
        $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
    )
endif()
target_compile_definitions(vmlib PRIVATE
    ${VSF_TARGET_DEFINITIONS}
    "main=iwasm_main"
    WASM_ENABLE_LIBC_VSFWASI=1
)

# for AIC8800, some os_xxx APIs conflicts with APIs in wamr
if(${VSF_HAL_CHIP_SERIES} STREQUAL AIC8800)
    target_compile_definitions(vmlib PRIVATE
        "os_malloc=__wamr_os_malloc"
        "os_realloc=__wamr_os_realloc"
        "os_free=__wamr_os_free"
    )
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC vmlib)
