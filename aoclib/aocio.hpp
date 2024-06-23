#pragma once

#include <iostream>
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

inline bool file_getlines(std::string_view fname, std::vector<std::string>& lines)
{
    std::ifstream file {fname};
    if (!file) {
        std::cerr << "Cannot open file " << fname << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return true;
}

inline void line_tokenise(const std::string& line, const std::string& delims, const std::string& preserved_delims, std::vector<std::string>& tokens)
{
    for (char d : preserved_delims) {
        if (delims.find(d) == std::string::npos) {
            throw std::invalid_argument("Preserved delim not in delims");
        }
    }
    std::string::size_type start_pos = 0;

    while (start_pos < line.size()) {
        auto token_end_pos = line.find_first_of(delims, start_pos); 
        if (token_end_pos == std::string::npos) {
            token_end_pos = line.size();
        }
        std::string token = line.substr(start_pos, token_end_pos - start_pos);
        if (token.size()) {
            tokens.push_back(token);
        }
        
        if (token_end_pos != std::string::npos && preserved_delims.size() && preserved_delims.find(line[token_end_pos]) != std::string::npos) {
            tokens.push_back(std::string{line[token_end_pos]});
        }

        start_pos = token_end_pos + 1;
    }
}

static inline std::optional<int> parse_num(const std::string &str)
{
    size_t num_read = 0; 
    int n = std::stoi(str, &num_read); 
    if (num_read == 0) {
        return {};
    }
    return n; 
} 

static inline std::optional<int64_t> parse_num_i64(const std::string& str)
{
    size_t num_read = 0; 
    int64_t n = std::stoll(str, &num_read); 
    if (num_read == 0) {
        return {};
    }
    return n; 
} 

static inline std::optional<int> parse_digit(char c)
{
    int digit = static_cast<int>(c) - '0'; 
    if (digit >= 0 && digit <= 9) {
        return digit; 
    } else {
        return {};
    }
}

template <typename IntT = int>
static inline std::optional<IntT> parse_hex(std::string_view str)
{
    if (str.size() == 0) {
        return {};
    }

    int prefix = 0; 
    while (str.at(prefix) == ' ' || str.at(prefix) == '\t') { // Remove leading whitespace
        ++prefix; 
    }
    str = str.substr(prefix, str.size());

    if (str.size() >= 2) { // Handle # and 0x prefixes
        if (str.at(0) == '#') {
            str = str.substr(1, str.size());
        } else if (str.size() >= 3) {
            if (str.at(0) == '0' && str.at(1) == 'x') {
                str = str.substr(2, str.size());
            }
        }
    } 
 
    int end_idx = std::ssize(str) - 1; 
    while (str.at(end_idx) == ' ' || str.at(end_idx) == '\t') { // Remove trailing whitespace. 
        end_idx -= 1; 
    }

    IntT res = 0; 
    IntT fac = 1; 
    for (int i = end_idx; i >= 0; --i, fac *= 16) {
        char sym = str.at(i); 
        IntT digit = 0; 
        if (sym >= '0' && sym <= '9') {
            digit = aocio::parse_digit(sym).value(); 
        } else {
            digit = 10 + (sym - 'a'); 
            if (digit < 10 || digit > 15) { // Try uppercase. 
                digit = 10 + (sym - 'A'); 
            }
            if (digit < 10 || digit > 15) {
                return {};
            }
        }
        res += digit * fac;
    }
    return res; 
}

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