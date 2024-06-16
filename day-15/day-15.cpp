#include <unordered_map>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/15
  
    Solutions: 
        - Part 1: 512283
        - Part 2: 
    Notes:  

*/

bool is_ascii(char c)
{
    return c >= 0 && c <= 127; 
}

size_t hash(std::string_view s)
{
    size_t h = 0; 
    for (char c : s) {
        if (!is_ascii(c)) {
            throw "hash: not an ASCII-character";
        }
        h += (uint8_t)c; 
        h *= 17; 
        h = h % 256; 
    }
    return h; 
}

size_t part_one(const std::vector<std::string>& lines)
{
    size_t total_hash = 0; 
    std::string_view line = lines.at(0);
    size_t start_idx = 0; 
    do {
        size_t end_idx = line.find(",", start_idx); 
        if (end_idx == std::string::npos) {
            end_idx = line.size();
        }
        assert(end_idx >= start_idx);
        std::string_view word = line.substr(start_idx, end_idx - start_idx); 
        total_hash += hash(word); 
        start_idx = end_idx + 1; 
    } while (start_idx < line.size());

    return total_hash;
}

size_t part_two(const std::vector<std::string>& lines)
{
    return 0;
}

int main()
{
    aocio::print_day();
    std::vector<std::string> lines;
    std::string_view fname = AOC_INPUT_DIR"input.txt";
    bool file_loaded = aocio::file_getlines(fname, lines);
    if (!file_loaded) {
        std::cerr << "Error: " << "File '" << fname << "' not found\n";
        return EXIT_FAILURE;
    }    
    try {
        size_t p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        size_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}