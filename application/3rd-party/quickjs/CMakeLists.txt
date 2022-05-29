# CMakeLists head
cmake_policy(SET CMP0079 NEW)

file(GLOB QUICKJS_SOURCES
    raw/cutils.c
    raw/libregexp.c
    raw/libunicode.c
    raw/quickjs.c
    raw/quickjs-libc.c
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    ${QUICKJS_SOURCES}
)
set(QUICKJS_INCLUDE_DIRECTORIES
    raw
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${QUICKJS_INCLUDE_DIRECTORIES}
)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    VSF_USE_QUICKJS=ENABLED
    CONFIG_VERSION="unknown"
)

if(APP_USE_MEUI_DEMO)
    file(GLOB QEMI_SOURCES
        MEUI/lib/flex/*.c
        MEUI/lib/hashmap/src/*.c
        MEUI/lib/plutosvg/source/*.c
        MEUI/lib/plutovg/source/*.c
        MEUI/lib/plutovg/3rdparty/software/*.c
        MEUI/lib/pqueue/*.c
        MEUI/lib/stb/*.c

        MEUI/src/*.c
        MEUI/src/platform/vsf/*.c
    )
    file(GLOB_RECURSE QEMI_SOURCES_RECURSE
        MEUI/src/bind/*.c
        MEUI/src/element/*.c
    )

    add_library(meui STATIC
        ${QEMI_SOURCES}
        ${QEMI_SOURCES_RECURSE}
    )
    target_compile_definitions(meui PRIVATE
        ${VSF_TARGET_DEFINITIONS}
        "main=meui_main"
    )
    target_include_directories(meui PRIVATE
        ${QUICKJS_INCLUDE_DIRECTORIES}
        ${VSF_COMMON_INCLUDE_DIRECTORIES}

        $ENV{VSF_PATH}/source/shell/sys/linux/include
        $ENV{VSF_PATH}/example/template/demo/common

        MEUI/include
        MEUI/lib/flex/include
        MEUI/lib/hashmap/include
        MEUI/lib/plutosvg/include
        MEUI/lib/plutovg/include
        MEUI/lib/plutovg/3rdparty/software
        MEUI/lib/pqueue
        MEUI/lib/stb/include
    )
    if(VSF_LINUX_USE_SIMPLE_LIBC)
    target_include_directories(meui PRIVATE
        $ENV{VSF_PATH}/source/shell/sys/linux/include/simple_libc
    )
    endif()
    target_link_libraries(meui PRIVATE
        m   # -lm
    )

    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        MEUI/include
    )
    target_link_libraries(${VSF_LIB_NAME} INTERFACE
        meui
    )
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        APP_USE_MEUI_DEMO=ENABLED
    )
endif()