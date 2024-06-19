#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <limits>
#include <cassert>
#include <optional>

#ifndef AOC_INPUT_PATH
#define AOC_INPUT_PATH ""
#endif

#ifndef AOC_INPUT_DIR
#define AOC_INPUT_DIR ""
#endif

namespace aocio 
{
bool file_getlines(std::string_view fname, std::vector<std::string>& lines);
void line_tokenise(const std::string& line, const std::string& delims, const std::string& preserved_delims, std::vector<std::string>& tokens);

std::optional<int> parse_num(const std::string &str);
std::optional<int64_t> parse_num_i64(const std::string& str);

inline void print_day() 
{
    std::string day_name {std::filesystem::path(AOC_INPUT_DIR).parent_path().filename()};
    
    if (day_name.size()) {
        day_name[0] = std::toupper(day_name[0]);
    }

    std::string debug_release;
    #ifdef NDEBUG
    debug_release = "Release";
    #else
    debug_release = "Debug";
    #endif

    std::cout << day_name << " (" << debug_release << ")\n";
}
}