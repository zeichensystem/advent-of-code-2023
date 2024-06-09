#include <string>
#include <unordered_map>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/6

    Solutions: 
        - Part 1: 
        - Part 2: 
        
    Notes: 
      
*/


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

    // try {
    //     int64_t p1 = part_one(lines);  
    //     std::cout << "Part 1: " << p1 << "\n";
    //     int p2 = part_two(lines);
    //     std::cout << "Part 2: " << p2 << "\n";
    // } catch (const char* err) {
    //     std::cerr << "Error: " << err << "\n";
    //     return EXIT_FAILURE;
    // }

    return EXIT_SUCCESS;
}