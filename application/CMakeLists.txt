# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    vsf_main.c
)

if(APP_USE_LINUX_CURL_DEMO)
    add_subdirectory(3rd-party/curl)
endif()
if(APP_USE_LINUX_BUSYBOX_DEMO)
    add_subdirectory(3rd-party/busybox)
endif()
if(APP_USE_LINUX_WAMR_DEMO)
    add_subdirectory(3rd-party/wamr)
endif()
if(APP_USE_SDL_DEMO)
    add_subdirectory(3rd-party/sdl)
endif()
if(APP_USE_QUICKJS_DEMO)
    add_subdirectory(3rd-party/quickjs)
endif()
add_subdirectory(usrapp)