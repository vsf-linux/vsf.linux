# CMakeLists head

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC
    APP_USE_USRAPP=ENABLED
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    .
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    usrapp_main.c
)
