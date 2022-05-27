# CMakeLists head

cmake_policy(SET CMP0079 NEW)

set(VSF_MBEDTLS_PATH ${VSF_SRC_PATH}/component/3rd-party/mbedtls/raw)
set(VSF_MBEDTLS_PORT_PATH ${VSF_SRC_PATH}/component/3rd-party/mbedtls/port)
file(GLOB_RECURSE CURL_SOURCES
    port/*.c
    raw/src/*.c
    raw/lib/*.c
    raw/lib/vauth/*.c
    raw/lib/vquic/*.c
    raw/lib/vssh/*.c
    raw/lib/vtls/*.c
)
file(GLOB MBEDTLS_LIB_SOURCES
    ${VSF_MBEDTLS_PATH}/library/*.c
)
add_library(libcurl STATIC
    ${CURL_SOURCES}
    ${MBEDTLS_LIB_SOURCES}

    ${VSF_MBEDTLS_PORT_PATH}/platform_vsf.c
)
target_compile_definitions(libcurl PRIVATE
    ${VSF_TARGET_DEFINITIONS}
    "main=curl_main"
    MBEDTLS_USER_CONFIG_FILE="component/3rd-party/mbedtls/port/platform_vsf.h"
    __VSF__
)
target_include_directories(libcurl PRIVATE
    port
    raw/lib
    raw/include

    ${VSF_COMMON_INCLUDE_DIRECTORIES}
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    ${VSF_MBEDTLS_CONFIG_PATH}
    ${VSF_MBEDTLS_PATH}/include
)
if(VSF_LINUX_USE_SIMPLE_LIBC)
target_include_directories(libcurl PRIVATE
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
)
endif()
if(VSF_TARGET_WRAPPER_LIST)
    foreach(api ${VSF_TARGET_WRAPPER_LIST})
        target_compile_definitions(libcurl PRIVATE
            "${api}=__wrap_${api}"
        )
    endforeach()
endif()


target_include_directories(${VSF_LIB_NAME} INTERFACE
    raw/include
    ${VSF_MBEDTLS_PATH}/include
)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/libcurl_bin)
target_link_libraries(${VSF_LIB_NAME} INTERFACE
    libcurl
)
target_compile_definitions(${VSF_LIB_NAME} INTERFACE
    APP_USE_LINUX_CURL_DEMO=ENABLED
)
