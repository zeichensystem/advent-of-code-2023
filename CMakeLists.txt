cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (APPLE OR UNIX OR LINUX)
    set(WARNING_FLAGS_CXX -Wall -Wextra -Wpedantic -Wvla -Wshadow -Wundef -Wmisleading-indentation -Wnull-dereference -Wswitch-default -Wno-newline-eof -Wno-unused-function -Wno-unused-parameter)
    set(CMAKE_CXX_COMPILER  "/usr/bin/clang++")
endif ()

project(aoc-2023)
set(CMAKE_DEBUG_POSTFIX _dbg)

set(TARGETS day-01)

foreach(current_target IN LISTS TARGETS)
    add_executable(${current_target} ${current_target}/${current_target}.cpp)
    set_target_properties(${current_target} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_include_directories(${current_target} PRIVATE $loop_var/) 
    target_compile_options(${current_target} PRIVATE ${WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og>)
    target_link_options(${current_target} PRIVATE ${WARNING_WARNING_FLAGS_CXX} $<$<CONFIG:Debug>:-fsanitize=undefined,address -g3 -Og> )

    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_available)
    if (ipo_available AND CMAKE_BUILD_TYPE EQUAL "RELEASE" OR CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
        set_target_properties(${current_target} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif() 

endforeach(current_target)


