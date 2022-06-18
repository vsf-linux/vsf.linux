# CMakeLists head
cmake_policy(SET CMP0079 NEW)

file(GLOB UPY_SOURCES
    raw/core/*.c
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    ${UPY_SOURCES}
)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    raw/include
    raw/core
)
