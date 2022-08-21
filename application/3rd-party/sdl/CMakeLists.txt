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
    raw/src/joystick/*.c
    raw/src/joystick/dummy/*.c
)
vsf_add_sources(
    ${SDL_SOURCES}
)
vsf_add_include_directories(
    include
    include/SDL2
    port
)
vsf_add_compile_definitions(
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
    vsf_add_sources(
        ${SDLPAL_SOURCES}
    )
    vsf_add_include_directories(
        sdlpal
        sdlpal/timidity
        sdlpal/vsf
    )
    vsf_add_compile_definitions(
        APP_USE_SDLPAL_DEMO=ENABLED
    )
endif()

if(APP_USE_SDL_MGBA_DEMO)
    file(GLOB MGBA_SOURCES
        port/mgba/*.c

        mgba/src/arm/*.c
        mgba/src/core/*.c

#        mgba/src/feature
            mgba/src/feature/commandline.c
            mgba/src/feature/thread-proxy.c
            mgba/src/feature/video-logger.c
        mgba/src/gb/*.c
            mgba/src/gb/extra/*.c
            mgba/src/gb/renderers/*.c
            mgba/src/gb/sio/*.c
        mgba/src/gba/*.c
            mgba/src/gba/cart/*.c
            mgba/src/gba/cheats/*.c
            mgba/src/gba/extra/*.c
            mgba/src/gba/cart/*.c
            mgba/src/gba/sio/*.c
#           mgba/src/gba/renderers
                mgba/src/gba/renderers/cache-set.c
                mgba/src/gba/renderers/common.c
                mgba/src/gba/renderers/software-bg.c
                mgba/src/gba/renderers/software-mode0.c
                mgba/src/gba/renderers/software-obj.c
                mgba/src/gba/renderers/video-software.c
#        mgba/src/platform/sdl
            mgba/src/platform/sdl/main.c
            mgba/src/platform/sdl/sdl-audio.c
            mgba/src/platform/sdl/sdl-events.c
            mgba/src/platform/sdl/sw-sdl2.c
        mgba/src/sm83/*.c

        mgba/src/third-party/blip_buf/*.c
        mgba/src/third-party/inih/*.c
        mgba/src/third-party/lzma/*.c

        mgba/src/util/*.c
        mgba/src/util/vfs/vfs-dirent.c
        mgba/src/util/vfs/vfs-fd.c
        mgba/src/util/vfs/vfs-fifo.c
        mgba/src/util/vfs/vfs-file.c
        mgba/src/util/vfs/vfs-lzma.c
        mgba/src/util/vfs/vfs-mem.c
    )
    vsf_add_sources(
        ${MGBA_SOURCES}
    )
    vsf_add_include_directories(
        mgba/include
        port/mgba
        mgba/src
    )
    vsf_add_compile_definitions(
        APP_USE_MGBA_DEMO=ENABLED
        _7ZIP_ST
    )
endif()
