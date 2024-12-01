#include <unordered_set>
#include <queue>
#include <numeric>

#include "../aoclib/aocio.hpp"
#include "../aoclib/grid.hpp"
#include "../aoclib/vec.hpp"

/*
    Problem: https://adventofcode.com/2023/day/21
  
    Solutions: 
        - Part 1: 3770
        - Part 2: TODO...
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

struct TileInfo {
    bool visited = false; 
    int distance = 0; 

    friend std::ostream& operator<<(std::ostream& os, const TileInfo& t)
    {
        if (t.visited) {
            return os << std::setw(3) << t.distance; 
        } else {
            return os << std::setw(3) << "#";
        }
    }
};

 Grid<TileInfo> calc_distances(const Grid<char>& grid)
{
    Vec2 start_pos = grid.find_elem_positions('S').at(0);

    Grid<TileInfo> grid_data; 
    for (int y = 0; y < grid.height(); ++y) {
        grid_data.push_row(std::vector<TileInfo>(grid.width(), TileInfo{})); 
    }

    std::queue<Vec2> q; 
    q.push(start_pos); 

    while (!q.empty()) {
        Vec2 v = q.front(); 
        q.pop();

        if (grid_data.at(v).visited) {
            continue;
        } else {
            grid_data.at(v).visited = true; 
        }

        int current_dist = grid_data.at(v).distance; 

        for (const Vec2& dir : aocutil::all_dirs_vec2<int>()) { 
            Vec2 adj_pos = v + dir; 
            auto sym = grid.try_get(adj_pos); 
            if (sym == '.' && !grid_data.at(adj_pos).visited) {
                grid_data.at(adj_pos).distance = current_dist + 1; 
                q.push(adj_pos); 
            }
        }
    }

    return grid_data; 
}

int64_t part_two(const std::vector<std::string>& lines)
{
    Grid<char> grid{lines}; 
    Grid<TileInfo> grid_data = calc_distances(grid); 

    int steps = 64; 

    int reachable = std::accumulate(grid_data.cbegin(), grid_data.cend(), 0, [steps](int total_reachable, const TileInfo& t) -> int {
        if (!t.visited || t.distance > steps) {
            return total_reachable; 
        } else if (t.distance == steps) {
            return total_reachable + 1; 
        } else if ((steps % 2) == (t.distance % 2)) {
            return total_reachable + 1;
        } else {
            return total_reachable;
        }
    });

    // std::cout << grid_data << "\n";

    auto and_op = [](bool a, bool b) -> bool {
        return a && b; 
    };
    auto same_mod2 = [](const TileInfo& a, const TileInfo& b) -> bool {
        return (a.distance % 2) == (b.distance % 2);
    };
    bool first_last_row_same_mod2 = std::inner_product(grid_data.cbegin_row(0), grid_data.cend_row(0), grid_data.cbegin_row(grid_data.height() - 1), true, and_op, same_mod2); 
    bool first_last_col_same_mod2 = std::inner_product(grid_data.cbegin_col(0), grid_data.cend_col(0), grid_data.cbegin_col(grid_data.width() - 1), true, and_op, same_mod2); 
    std::cout << "Row top and bottom same evenness: " << first_last_row_same_mod2 << "\n";
    std::cout << "Col left and right same evenness: " << first_last_col_same_mod2 << "\n";

    std::cout << "reachable: " << reachable << "\n";

    Vec2 start_pos = grid.find_elem_positions('S').at(0); 
    auto it_vert = std::find_if(grid.cbegin_col(start_pos.x), grid.cend_col(start_pos.x), [](char sym) {return sym == '#';});
    if (it_vert != grid.cend_col(start_pos.x)) {
        std::cout << "Grid: No straight vertical path to edge...\n";
    } else {
        std::cout << "Grid: Straight vertical path to edge!\n";
    }

    auto it_horiz = std::find_if(grid.cbegin_row(start_pos.y), grid.cend_row(start_pos.y), [](char sym) {return sym == '#';});
    if (it_horiz != grid.cend_row(start_pos.y)) {
        std::cout << "Grid: No straight horizontal path to edge...\n";
    } else {
        std::cout << "Grid: Straight horizontal path to edge!\n";
    }
    return -1; 
}

int main()
{
    aocio::print_day();
    std::vector<std::string> lines;
    std::string_view fname = AOC_INPUT_PATH;
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