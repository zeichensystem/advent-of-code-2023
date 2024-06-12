#include <string>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <array>
#include <queue>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/10

    Solutions: 
        - Part 1: 6812
        - Part 2: 527
    Notes: 
        - Part 1: Approach is a simple BFS of a cyclical graph where each vertex has degree 2. 

        - Part 2: It would have been a pretty straightforward flood-fill (BFS or DFS doesn't matter) if it hadn't
                  been for the "squeezing between pipes is also allowed!" part of the problem.

                  I "solved" that by first adding free inbetween '.' tiles between each whole tile, 
                  and then re-connecting prevoiusly connected pipes by filling the appropriate inbetween-tiles
                  with '-' or '|'. This way, only those tiles which lie between pipes that were not previously connected 
                  remain as free '.' inbetween tiles.

                  After this pre-processing step, we can finally run the aforementioned flood-fill.

                  My code got immeasurably horrible for Part 2 and I don't want to touch it anymore. 
                  But I'm thankful it worked.
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

constexpr char GRID_UNVISITED_SYM = 'x'; 

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

    if (p1 == GRID_UNVISITED_SYM || p2 == GRID_UNVISITED_SYM) {
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

    int find_loop_verts(bool mark_unvisited = false)
    {
        loop_verts_dist.clear(); 
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

             // TODO: use the pipes_connect function we already used instead, should be easier. 
             if (vert == start_vert) { // Find the correct pipe symbol for the 'S' for part 2 (I'm sorry).
                assert(neighbors.size() == 2);
                GridPos vpos = vertname_to_gridpos(vert); 
                GridPos n1 = neighbors.at(0);
                GridPos n2 = neighbors.at(1); 
                char n1_sym = grid.at(n1.y).at(n1.x); 
                char n2_sym = grid.at(n2.y).at(n2.x); 

                bool n1_sym_bend = (n1_sym == 'F' || n1_sym == '7' || n1_sym == 'J' || n1_sym == 'L'); 
                bool n2_sym_bend = (n2_sym == 'F' || n2_sym == '7' || n2_sym == 'J' || n2_sym == 'L'); 
                if (n1_sym == '-' && n2_sym == '-') {
                    grid.at(vpos.y).at(vpos.x) = '-'; 
                } else if (n1_sym == '|' && n2_sym == '|') {
                    grid.at(vpos.y).at(vpos.x) = '|'; 
                } 
                else if (n1_sym == '-' && n2_sym == '|' && n2.y < n1.y && n2.x > n1.x) {
                    grid.at(vpos.y).at(vpos.x) = 'J'; 
                } else if (n1_sym == '-' && n2_sym == '|' && n2.y < n1.y && n2.x < n1.x) {
                    grid.at(vpos.y).at(vpos.x) = 'L'; 
                } 
                else if (n1_sym == '-' && n2_sym == '|' && n2.y > n1.y && n2.x > n1.x) {
                    grid.at(vpos.y).at(vpos.x) = '7'; 
                } else if (n1_sym == '-' && n2_sym == '|' && n2.y > n1.y && n2.x < n1.x) {
                    grid.at(vpos.y).at(vpos.x) = 'F'; 
                } 
                else if ((n1_sym_bend && n2_sym == '-') || (n2_sym_bend && n1_sym == '-')) {
                    grid.at(vpos.y).at(vpos.x) = '-'; 
                } else if ((n1_sym_bend && n2_sym == '|') || (n2_sym_bend && n1_sym == '|')) {
                    grid.at(vpos.y).at(vpos.x) = '|'; 
                }

                else if (n1.x == n2.x) {
                    grid.at(vpos.y).at(vpos.x) = '|'; 
                } else if (n1.y == n2.y) {
                    grid.at(vpos.y).at(vpos.x) = '-'; 
                }
                Direction dir_x = (vpos.x < n1.x || vpos.x < n2.x) ? Direction::East : Direction::West;
                Direction dir_y = (vpos.y < n1.y || vpos.y < n2.y ) ? Direction::South : Direction::North;
                std::pair<Direction, Direction> dir = {dir_y, dir_x}; 
                for (const auto& [pipe_sym, d] : pipe_to_dir) {
                    if (dir.first == d.first && dir.second == d.second) {
                        grid.at(vpos.y).at(vpos.x) = pipe_sym;
                    }
                }
             }
    
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

        if (mark_unvisited) {
            for (int y = 0; y < std::ssize(grid); ++y) {
                for (int x = 0; x < std::ssize(grid.at(y)); ++x) {
                    GridPos pos {.x = x, .y = y}; 
                    if (!loop_verts_dist.contains(gridpos_to_vertname(pos))) {
                        grid.at(y).at(x) = GRID_UNVISITED_SYM; 
                        
                    }
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
    Graph g; 
    parse_grid(lines, g); 
    g.find_loop_verts(true); 

    // for (const auto& line : g.grid) {
    //     for (char c: line) {
    //         std::cout << c;
    //     }
    //     std::cout << "\n";
    // }

    // Add "inbetween" tiles between each tile. 
    std::vector<std::string> double_res_grid; 
    for (int y = 0; y < std::ssize(g.grid); ++y) {
        std::string dbl_line;
        std::string pt_line; 
        for (int x = 0; x < std::ssize(g.grid.at(y)); ++x) {
            dbl_line += g.grid.at(y).at(x); 
            pt_line += ".";
            if (x != std::ssize(g.grid.at(y)) - 1) {
                dbl_line += "."; 
                pt_line += ".";
            }
        }
        double_res_grid.push_back(dbl_line);
        if (y != std::ssize(g.grid) - 1) {
            double_res_grid.push_back(pt_line);
        }
    }
     for (int y = 0; y < std::ssize(double_res_grid); y+=1) { //  Re-connect pipes horizontally. 
        std::string dbl_line;
        std::string pt_line; 
        for (int x = 0; x < std::ssize(double_res_grid.at(y)); x+=1) {
            if (x % 2 == 1) {
                assert(double_res_grid.at(y).at(x) == '.'); 
                int left = x - 1; 
                int right = x + 1; 
                char left_sym = (double_res_grid.at(y).at(left));
                char right_sym = (double_res_grid.at(y).at(right));
                if (left_sym == GRID_UNVISITED_SYM && right_sym == GRID_UNVISITED_SYM) {
                    continue;
                } else if (left_sym == '-' && right_sym == '-') {
                    double_res_grid.at(y).at(x) = '-';
                } 
                else if (((left_sym == 'L' || left_sym == 'F')) && right_sym == '-') {
                    double_res_grid.at(y).at(x) = '-';
                } else if (((right_sym == '7' || right_sym == 'J')) && left_sym == '-') {
                    double_res_grid.at(y).at(x) = '-';
                } 
                else if (left_sym == 'L' && (right_sym == 'J' || right_sym == '7') ) {
                    double_res_grid.at(y).at(x) = '-';
                } else if (left_sym == 'F' && (right_sym == 'J' || right_sym == '7')) {
                    double_res_grid.at(y).at(x) = '-';
                }
            }
        }
    }
    for (int y = 0; y < std::ssize(double_res_grid); y+=1) { // Re-connect pipes vertically. 
        std::string dbl_line;
        std::string pt_line; 
        for (int x = 0; x < std::ssize(double_res_grid.at(y)); x+=1) {
            if (y % 2 == 1) {
                assert(double_res_grid.at(y).at(x) == '.'); 
                int top = y - 1; 
                int bottom = y + 1; 
                char top_sym = (double_res_grid.at(top).at(x));
                char bottom_sym = (double_res_grid.at(bottom).at(x));
                if (top_sym == GRID_UNVISITED_SYM && bottom_sym == GRID_UNVISITED_SYM) {
                    continue;
                } else if (top_sym == '|' && bottom_sym == '|') {
                    double_res_grid.at(y).at(x) = '|';
                } 
                else if (((bottom_sym == 'L' || bottom_sym == 'J')) && top_sym == '|') {
                    double_res_grid.at(y).at(x) = '|';
                } else if (((top_sym == '7' || top_sym == 'F')) && bottom_sym == '|') {
                    double_res_grid.at(y).at(x) = '|';
                }
                else if (top_sym == '7' && (bottom_sym == 'L' || bottom_sym == 'J') ) {
                    double_res_grid.at(y).at(x) = '|';
                } else if (top_sym == 'F' && (bottom_sym == 'L' || bottom_sym == 'J')) {
                    double_res_grid.at(y).at(x) = '|';
                }
            }
        }
    }

    g.grid = double_res_grid; 

    // Find start vertices on the edge of the grid (only those which don't fall on inbetween-tiles).
    std::vector<std::string> start_verts; 
    for (int y = 0; y < std::ssize(g.grid); ++y) {
        if (y == 0 || y == std::ssize(g.grid) - 1) { 
            for (int x = 0; x < std::ssize(g.grid.at(y)); ++x) {
                if (g.grid.at(y).at(x) == GRID_UNVISITED_SYM) {
                    std::string vert = gridpos_to_vertname(GridPos{.x=x, .y=y}); 
                    start_verts.emplace_back(vert); 
                }
            }
            continue; 
        }
        if (g.grid.at(y).front() == GRID_UNVISITED_SYM) {
            std::string vert = gridpos_to_vertname(GridPos{.x=0, .y=y}); 
            start_verts.emplace_back(vert); 
        }
        if (g.grid.at(y).back() == GRID_UNVISITED_SYM) {
            std::string vert = gridpos_to_vertname(GridPos{.x=static_cast<int>(g.grid.at(y).size()) - 1, .y=y}); 
            start_verts.emplace_back(vert); 
        }
    }

    // Flood fill (this time it's a DFS, but a BFS with a queue would also work.) 
    std::unordered_set<std::string> visited; 
    const char REACHABLE_SYM = 'o';
    for (const auto& s_vert: start_verts) {
        if (visited.contains(s_vert)) {
            continue; 
        } 
        std::stack<std::string> vert_stack; 
        vert_stack.push(s_vert); 

        while (!vert_stack.empty()) {
            std::string vert = vert_stack.top();
            vert_stack.pop(); 
            GridPos vpos = vertname_to_gridpos(vert); 

            if (!visited.contains(vert)) {
                visited.insert(vert); 
                if (g.grid.at(vpos.y).at(vpos.x) == GRID_UNVISITED_SYM) { // Only modify on whole tiles.
                    g.grid.at(vpos.y).at(vpos.x) = REACHABLE_SYM; 
                } 
            } else {
                continue;
            }

            for (Direction d : all_dirs) { // Find neighbors.
                GridPos adj_pos; 
                std::string adj_vert; 
                auto adj_sym = g.grid_peek(vpos, d, adj_pos); 
                if (adj_sym) {
                    adj_vert = gridpos_to_vertname(adj_pos); 
                    if (visited.contains(adj_vert)) {
                        continue; 
                    } else {
                        bool wall = adj_sym.value() == '|' || adj_sym.value() == '-' || adj_sym.value() == 'L' || adj_sym.value() == 'J'|| adj_sym.value() == '7' || adj_sym.value() == 'F'; 
                        if (!wall) {
                            vert_stack.push(adj_vert);  
                            if (g.grid.at(vpos.y).at(vpos.x) == GRID_UNVISITED_SYM) { // Only modify grid for whole tiles, not inbetween-tiles.
                                g.grid.at(vpos.y).at(vpos.x) = REACHABLE_SYM; 
                            } 
                        }
                    }
                }
            }
        }
    }

    int count_unreachable = 0; 
    for (int y = 0; y < std::ssize(g.grid); y+= 2) { // Only sample whole tiles, not inbetween-tiles. 
        for (int x = 0; x < std::ssize(g.grid.at(y)); x+= 2) {
            if (g.grid.at(y).at(x) == GRID_UNVISITED_SYM) {
                ++count_unreachable; 
            }
        }
    }

    // for (const auto& line : g.grid) {
    //     for (char c : line) {
    //         std::cout << c;
    //     }
    //     std::cout << "\n";
    // }

    return count_unreachable; 
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