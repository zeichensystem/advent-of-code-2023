#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <limits>
#include <cassert>

#include "aocio.hpp"

#ifndef AOC_INPUT_PATH
#define AOC_INPUT_PATH ""
#endif

#ifndef AOC_INPUT_DIR
#define AOC_INPUT_DIR ""
#endif

namespace aocio 
{
bool file_getlines(std::string_view fname, std::vector<std::string>& lines);

inline void print_day() 
{
    std::string day_name {std::filesystem::path(AOC_INPUT_DIR).parent_path().filename()};

    std::string debug_release;
    #ifdef NDEBUG
    debug_release = "Release";
    #else
    debug_release = "Debug";
    #endif

    std::cout << day_name << " (" << debug_release << ")\n";
}
}
