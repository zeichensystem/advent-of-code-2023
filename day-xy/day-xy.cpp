#include <unordered_map>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/N
  
    Solutions: 
        - Part 1: 
        - Part 2: 
    Notes:  
        - Part 1: 
        - Part 2:
*/

int64_t part_one(const std::vector<std::string>& lines)
{
    return -1;
}

int64_t part_two(const std::vector<std::string>& lines)
{
    return -1; 
}

int main()
{
    aocio::print_day();
    std::vector<std::string> lines;
    std::string_view fname = AOC_INPUT_DIR"input-example.txt";
    bool file_loaded = aocio::file_getlines(fname, lines);
    if (!file_loaded) {
        std::cerr << "Error: " << "File '" << fname << "' not found\n";
        return EXIT_FAILURE;
    }    

    aocio::remove_leading_empty_lines(lines);
    aocio::remove_trailing_empty_lines(lines);
    if (!lines.size()) {
        std::cerr << "Error: " << "Input is empty";
        return EXIT_FAILURE;
    }

    try {
        int64_t p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        int64_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}