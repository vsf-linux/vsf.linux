# CMakeLists head

cmake_policy(SET CMP0079 NEW)

file(GLOB_RECURSE SOURCES raw/src/*.c raw/lib/*.c raw/lib/vauth/*.c raw/lib/vquic/*.c raw/lib/vssh/*.c raw/lib/vtls/*.c)
add_library(libcurl STATIC
    ${SOURCES}
    port/curl_port_vsf.c
)
target_compile_definitions(libcurl PRIVATE
    ${VSF_TARGET_DEFINITIONS}
    "main=curl_main"
    __VSF__
)
if(NOT VSF_MBEDTLS_PATH)
    set(VSF_MBEDTLS_PATH ${VSF_SRC_PATH}/component/3rd-party/mbedtls/raw)
endif()
target_include_directories(libcurl PRIVATE
    port
    raw/lib
    raw/include

    ${VSF_COMMON_INCLUDE_DIRECTORIES}
    $ENV{VSF_PATH}/source/shell/sys/linux/include
    ${VSF_MBEDTLS_PATH}/include
)
if(VSF_LINUX_USE_SIMPLE_LIBC)
target_include_directories(libcurl PRIVATE
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
)
endif()


target_include_directories(${VSF_LIB_NAME} INTERFACE
    raw/include
)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/libcurl_bin)
target_link_libraries(${VSF_LIB_NAME} INTERFACE
    libcurl
)
