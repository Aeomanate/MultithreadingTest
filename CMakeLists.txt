cmake_minimum_required(VERSION 3.13)
project(Test-CLion)

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

set(STATIC_BUILD FALSE)
if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(STATIC_BUILD TRUE)
endif ()

if (STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Executable/bin)
set(ENV_ROOT "D:/Programms/ProgrammingEnvironment/")


add_executable(${PROJECT_NAME}
    main.cpp

    )


set(SFML_ROOT "${ENV_ROOT}/Librares/SFML-2.5.1")
set(SFML_STATIC_LIBRARIES ${STATIC_BUILD})
set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system sfml-network)


set(GMP_DIR "${ENV_ROOT}/Librares/gmp-6.1.1")
include_directories(${PROJECT_NAME} ${GMP_DIR}/include)
target_link_libraries(${PROJECT_NAME}
    "${GMP_DIR}/lib/libgmp.dll.a"
    "${GMP_DIR}/lib/libgmpxx.dll.a"
    )


set(CONFIG_LOCATION "${ENV_ROOT}/Utility/PVS-Studio/PVS-Studio.cfg")
if (EXISTS ${CONFIG_LOCATION})
    file(READ "${CONFIG_LOCATION}" config_content)
endif ()
if (config_content STREQUAL "")
    file(
        WRITE
        "${ENV_ROOT}/Utility/PVS-Studio/PVS-Studio.cfg"
        "exclude-path = ${ENV_ROOT}/Compilers \nexclude-path = ${ENV_ROOT}/Librares"
    )
endif ()

include(${ENV_ROOT}/Utility/PVS-Studio/PVS-Studio.cmake)
pvs_studio_add_target(
    CONFIG ${ENV_ROOT}/Utility/PVS-Studio/PVS-Studio.cfg
    TARGET ${PROJECT_NAME}.analyze ALL
    OUTPUT FORMAT errorfile
    ANALYZE ${PROJECT_NAME}
    MODE GA 64 OP
    LOG target.err
)
