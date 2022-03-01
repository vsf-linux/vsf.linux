# CMakeLists head

cmake_policy(SET CMP0076 NEW)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    vsf_main.c
)
