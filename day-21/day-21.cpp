#include <unordered_set>
#include "../aoclib/aocio.hpp"
#include "../aoclib/grid.hpp"
#include "../aoclib/vec.hpp"

/*
    Problem: https://adventofcode.com/2023/day/21
  
    Solutions: 
        - Part 1: 3770
        - Part 2: 
    Notes:  
        - Part 1: 
        - Part 2:
*/

using aocutil::Grid; 
using Vec2 = aocutil::Vec2<int>;

int64_t num_reachable(const Grid<char>& grid, int steps)
{
    Vec2 start_pos = grid.find_elem_positions('S').at(0);
    std::unordered_set<Vec2> positions {start_pos};

    for (int i = 0; i < steps; ++i) {
        std::unordered_set<Vec2> reached; 
        for (const Vec2& pos : positions) {
            for (const Vec2& dir : aocutil::all_dirs_vec2<int>()) { // Try all neighbors. 
                Vec2 adj_pos = pos + dir;
                auto sym = grid.try_get(adj_pos); 
                if (sym == '.' || sym == 'S') {
                    reached.insert(adj_pos);
                }
            }
        }
        positions = reached; 
    }

    return positions.size();
}

int64_t part_one(const std::vector<std::string>& lines)
{
    Grid<char> grid{lines}; 
    return num_reachable(grid, 64); 
}

int64_t part_two(const std::vector<std::string>& lines)
{
    return -1; 
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