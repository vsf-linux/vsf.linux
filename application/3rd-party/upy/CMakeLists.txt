# CMakeLists head
cmake_policy(SET CMP0079 NEW)

file(GLOB UPY_SOURCES
    raw/core/*.c
    raw/*.c
)
add_library(upy STATIC
    ${UPY_SOURCES}
)
target_compile_definitions(upy PRIVATE
    ${VSF_TARGET_DEFINITIONS}
    "main=upy_main"
    __VSF__
)
target_include_directories(upy PRIVATE
    raw/include
    raw/core

    ${VSF_COMMON_INCLUDE_DIRECTORIES}
    $ENV{VSF_PATH}/source/shell/sys/linux/include
)
if(VSF_LINUX_USE_SIMPLE_LIBC)
target_include_directories(upy PRIVATE
    $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
)
endif()
target_compile_definitions(upy PRIVATE
    "-lm"
)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/upy_bin)
target_link_libraries(${VSF_LIB_NAME} INTERFACE
    upy
)
target_compile_definitions(${VSF_LIB_NAME} INTERFACE
    APP_USE_UPY_DEMO=ENABLED
)
