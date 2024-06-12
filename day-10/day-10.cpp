#include <string>
#include <unordered_map>
#include <numeric>
#include <array>
#include <queue>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/10

    Solutions: 
        - Part 1: 6812
        - Part 2:      
    Notes: 

*/

struct GridPos {
    int x, y; 
};

enum class Direction {North=0, East=1, South=2, West=3}; 
const std::array<Direction,4> all_dirs = {Direction::North, Direction::East, Direction::South, Direction::West};

const std::unordered_map<char, std::pair<Direction, Direction>> pipe_to_dir {
    {'|', {Direction::North, Direction::South}}, 
    {'-', {Direction::East, Direction::West}}, 
    {'L', {Direction::North, Direction::East}}, 
    {'J', {Direction::North, Direction::West}}, 
    {'7', {Direction::South, Direction::West}}, 
    {'F', {Direction::South, Direction::East}}, 
};

Direction dir_opposite(Direction d)
{
    switch (d) {
        case Direction::North:
            return Direction::South; 
        case Direction::South: 
            return Direction::North; 
        case Direction::East: 
            return Direction::West; 
        case Direction::West: 
            return Direction::East;
        default:
            throw "Invalid direction"; 
    }
}

bool pipes_connect(char p1, char p2, Direction p1_rel_dir)
{
    if (p1 == 'S' && p2 == 'S') {
        throw "Two start vertices"; 
    } 
    if (p1 == '.' || p2 == '.') {
        return false; 
    }

    std::optional<std::pair<Direction, Direction>> p1_dir {}; 
    std::optional<std::pair<Direction, Direction>> p2_dir {}; 
    if (pipe_to_dir.contains(p1)) {
        p1_dir = pipe_to_dir.at(p1); 
    }  else if (p1 != 'S') {
        throw "Invalid pipe symbol";
    }
    
    if (pipe_to_dir.contains(p2)) {
        p2_dir = pipe_to_dir.at(p2); 
    } else if (p2 != 'S') {
        throw "Invalid pipe symbol"; 
    }

    if (!p1_dir && !p2_dir) {
        throw "Invalid pipe symbol"; 
    }

    if (p2_dir) {
        if (p2_dir.value().first != p1_rel_dir && p2_dir.value().second != p1_rel_dir) {
            return false; 
        }
    } else {
        assert(p1_dir); 
        if (p1_dir.value().first != dir_opposite(p1_rel_dir) && p1_dir.value().second != dir_opposite(p1_rel_dir)) {
            return false; 
        }
    }

    if (p1 == 'S' || p2 == 'S') {
        return true; 
    }

    if (!p1_dir || !p2_dir) {
        throw "Should not happen";
    }

    return dir_opposite(p1_dir.value().first) == (p2_dir.value().first) || dir_opposite(p1_dir.value().first) == (p2_dir.value().second) ||
           dir_opposite(p1_dir.value().second) == (p2_dir.value().first) || dir_opposite(p1_dir.value().second) == p2_dir.value().second; 
}

GridPos vertname_to_gridpos(const std::string& vert)
{
    auto sep_idx = vert.find(",");
    assert(sep_idx != std::string::npos); 
    assert(sep_idx < vert.size() - 1); 
    int x =  aocio::parse_num(vert.substr(0, sep_idx)); 
    int y = aocio::parse_num(vert.substr(sep_idx + 1, vert.size())); 
    return GridPos {x, y}; 
}

std::string gridpos_to_vertname(const GridPos& pos)
{
    std::string vert; 
    vert = std::to_string(pos.x) + "," + std::to_string(pos.y); 
    return vert; 
}

struct Graph {
    std::string start_vert {}; 
    std::vector<std::string> grid {}; 
    std::unordered_map<std::string, int> loop_verts_dist {}; 

    GridPos start_pos()
    {
        return vertname_to_gridpos(start_vert);
    }

    char grid_get_sym(const GridPos& pos)
    {
        return grid.at(pos.y).at(pos.x); 
    }

    bool pos_on_grid(const GridPos &pos)
    {
        bool in_y = pos.y >= 0 && pos.y < std::ssize(grid); 
        if (!in_y) {
            return false; 
        }
        bool in_x = pos.x >= 0 && pos.x < std::ssize(grid.at(pos.y)); 
        return in_x; 
    }

    std::optional<char> grid_peek(const GridPos &pos, Direction d, GridPos &new_pos)
    {
        GridPos peek_pos = pos; 
        switch (d)
        {
        case Direction::North:
            peek_pos.y -= 1; 
            break; 
        case Direction::East:
            peek_pos.x += 1; 
            break; 
        case Direction::South:
            peek_pos.y += 1; 
            break; 
        case Direction::West: 
            peek_pos.x -= 1; 
            break; 
        default:
            throw "Invalid direction";
            break;
        }

        if (pos_on_grid(peek_pos)) {
            new_pos = peek_pos; 
            return grid_get_sym(peek_pos); 
        } else {
            new_pos = pos; 
            return {}; 
        }
    }

    void find_adjacent(const GridPos& pos, std::vector<GridPos>& neighbors)
    {
        char grid_sym = grid_get_sym(pos); 
        for (Direction d : all_dirs) {
            GridPos peek_pos; 
            auto neighbor_sym = grid_peek(pos, d, peek_pos); 
            if (neighbor_sym && pipe_to_dir.contains(neighbor_sym.value()) && pipes_connect(neighbor_sym.value(), grid_sym, d)) {
                neighbors.push_back(peek_pos); 
            }
        }
        assert(neighbors.size() <= 2); 
    }

    int find_loop_verts()
    {
        // Simple breadth-first-search of the cyclical graph. 
        loop_verts_dist.insert({start_vert, 0});
        std::queue<std::string> vert_queue;
        vert_queue.push(start_vert);

        while (!vert_queue.empty()) {
            std::string vert = vert_queue.front(); 
            vert_queue.pop(); 

            int dist = loop_verts_dist.at(vert); 

            std::vector<GridPos> neighbors; 
            find_adjacent(vertname_to_gridpos(vert), neighbors);
            for (const auto& pos : neighbors) {
                std::string vert_name = gridpos_to_vertname(pos); 
                if (loop_verts_dist.contains(vert_name)) { // Already visited.
                    continue; 
                } else {
                    loop_verts_dist.insert({vert_name, dist + 1}); 
                    vert_queue.push(vert_name); 
                }
            } 
        }
        auto max_dist_it = std::max_element(loop_verts_dist.begin(), loop_verts_dist.end(), [](const auto & a, const auto& b) -> bool {return a.second < b.second;} ); 
        if (max_dist_it == loop_verts_dist.end()) {
            throw "Could not find max dist"; 
        } 
        return max_dist_it->second; 
    }
};

void parse_grid(const std::vector<std::string>& lines, Graph &result)
{
    std::string start_vert {""}; 
    int y = 0; 
    for (const std::string &line : lines) {
        if (!line.size()) {
            ++y; 
            continue; 
        }
        auto idx = line.find('S'); 
        if (idx != std::string::npos) {
            assert(start_vert == ""); 
            int x = static_cast<int>(idx); 
            start_vert = gridpos_to_vertname(GridPos{x, y});  
        }
        result.grid.push_back(line); 
        ++y; 
    }
    assert(start_vert != "");

    result.start_vert = start_vert; 
}

int part_one(const std::vector<std::string>& lines)
{
    Graph g; 
    parse_grid(lines, g); 
    int max_dist = g.find_loop_verts(); 
    return max_dist;
}

int part_two(const std::vector<std::string>& lines)
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
    try {
        int p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        int p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}