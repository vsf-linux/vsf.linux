# CMakeLists head
cmake_policy(SET CMP0079 NEW)

file(GLOB SDL_SOURCES
    port/*.c
    port/src/audio/vsf/*.c
    port/src/main/vsf/*.c
    port/src/video/vsf/*.c
    raw/src/*.c
    raw/src/atomic/*.c
    raw/src/audio/*.c
    raw/src/audio/dummy/*.c
    raw/src/cpuinfo/*.c
    raw/src/events/*.c
    raw/src/file/*.c
    raw/src/filesystem/unix/*.c
    raw/src/render/*.c
    raw/src/render/software/*.c
    raw/src/stdlib/*.c
    raw/src/thread/*.c
    raw/src/thread/pthread/*.c
    raw/src/timer/*.c
    raw/src/timer/unix/*.c
    raw/src/video/*.c
    raw/src/video/dummy/*.c
    raw/src/video/yuv2rgb/*.c
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    ${SDL_SOURCES}
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    include
    include/SDL2
    port
)
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    SIGWINCH=28
    VSF_USE_SDL2=ENABLED
    VSF_APP_ENTRY=SDL_main
)

if(APP_USE_SDL_SDLPAL_DEMO)
    file(GLOB SDLPAL_SOURCES
        sdlpal/*.c
        sdlpal/*.cpp
        sdlpal/adplug/*.c
        sdlpal/adplug/*.cpp
        sdlpal/timidity/*.c
    )
    target_sources(${CMAKE_PROJECT_NAME} PUBLIC
        ${SDLPAL_SOURCES}
    )
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        sdlpal
        sdlpal/timidity
        sdlpal/vsf
    )
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
        APP_USE_SDLPAL_DEMO=ENABLED
    )
endif()