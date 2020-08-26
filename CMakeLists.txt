cmake_minimum_required(VERSION 3.13)
project(Test-MultithreadingCPP)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
        -std=gnu++17 \
        -pedantic \
        -Wall \
        -Wextra\
         \
        -Wconversion \
        -Wenum-compare \
        -Wfloat-equal \
        -Woverloaded-virtual \
    "
    )

set(STATIC_BUILD TRUE)
if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(STATIC_BUILD TRUE)
endif ()

if (STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Executable/bin)

add_executable(${PROJECT_NAME} main.cpp)
