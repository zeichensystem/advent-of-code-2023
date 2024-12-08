# Select debug/release variant: within build/Release or build/Debug, run: % cmake -DCMAKE_BUILD_TYPE=Debug/Release ../..
# Build and run: make run-day-XX or cmake --build . --target run_

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE OR UNIX OR LINUX)
    set(WARNING_FLAGS_CXX -Wall -Wextra -Wpedantic -Wvla -Wshadow -Wundef -Wmisleading-indentation -Wnull-dereference -Wshadow  -Wundef  -Wstrict-overflow=5 -Wsign-promo -Wcast-align -Wcast-qual -Woverloaded-virtual -Wredundant-decls -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wswitch-default -Wno-newline-eof -Wno-unused-function -Wno-unused-parameter)
    set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
endif ()

project(aoc-2023)
set(CMAKE_DEBUG_POSTFIX _dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin") 
set(CMAKE_VERBOSE_MAKEFILE OFF)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_available)

# add_library(aocio ${CMAKE_CURRENT_SOURCE_DIR}/aoclib/aocio.cpp)
# aocio
# set_target_properties(aocio PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
# target_include_directories(aocio PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/aoclib/") 
# target_compile_options(aocio PRIVATE ${WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og>)
# target_link_options(aocio PRIVATE ${WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og>)
# if(ipo_available AND (NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
#     message("-- IPO enabled")
#     set_property(TARGET aocio PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()

if(ipo_available AND (NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
    message("-- IPO enabled")
endif()

set(TARGETS day-01 day-02 day-03 day-04 day-05 day-06 day-07 day-08 day-09 day-10 day-11 day-12 day-13 day-14 day-15 day-16 day-17 day-18 day-19 day-20 day-21) # Add the other days as you please.

list(LENGTH TARGETS NUM_TARGETS)

foreach(current_target IN LISTS TARGETS)
    add_executable(${current_target} ${current_target}/${current_target}.cpp)
    set_target_properties(${current_target} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_include_directories(${current_target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/aoclib/") 
    target_compile_options(${current_target} PRIVATE ${WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og>)
    target_link_options(${current_target} PRIVATE ${WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og>)
    # target_link_libraries(${current_target} aocio)

    target_compile_definitions(${current_target} PRIVATE AOC_INPUT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/input/${current_target}.txt")
    target_compile_definitions(${current_target} PRIVATE AOC_INPUT_EXAMPLE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/input/${current_target}-example.txt")

    target_compile_definitions(${current_target} PRIVATE AOC_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/${current_target}/")

    if(ipo_available AND (NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
        set_property(TARGET ${current_target} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
    
    add_custom_target("run-${current_target}"
        DEPENDS ${current_target}
        COMMAND ${current_target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
    )
endforeach(current_target)

# TODO...
add_custom_target("run-all"
    DEPENDS ${TARGETS}
    COMMAND day-01
    COMMAND day-02
    COMMAND day-03
    COMMAND day-04
    COMMAND day-05
    COMMAND day-06
    COMMAND day-07
    COMMAND day-08
    COMMAND day-09
    COMMAND day-10
    COMMAND day-11
    COMMAND day-12
    COMMAND day-13
    COMMAND day-14
    COMMAND day-15
    COMMAND day-16
    COMMAND day-17
    COMMAND day-18
    COMMAND day-19
    COMMAND day-20
    COMMAND day-21

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
