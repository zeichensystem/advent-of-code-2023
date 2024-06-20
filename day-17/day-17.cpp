#include <unordered_set>
#include <stack>
#include "../aoclib/aocio.hpp"
#include "../aoclib/grid.hpp"
#include "../aoclib/prio-queue.hpp"

/*
    Problem: https://adventofcode.com/2023/day/17
  
    Solutions: 
        - Part 1: 907
        - Part 2: 
    Notes:  
        - Implemented a priority queue for aocutil
        - Manhattan distance does not seem to be a good heuristic here and runs even slower. (Or maybe I implemented something wrong...) 
        - Found out the way I combined hashes (i.e. h_combined = h1 ^ (h2 << 1), cf. the example code from  https://en.cppreference.com/w/cpp/utility/hash)
          was really bad. Using a hash_combine function I found produces better hashes and therefore makes code using maps etc. way faster.
          When using hash_combine, part 1 runs in ~0.5 seconds instead of ~6 seconds.
*/

using aocutil::Vec2; 
using aocutil::Grid; 
using aocutil::PrioQueue;

constexpr Vec2<int> dir_right = {.x = 1, .y = 0};
constexpr Vec2<int> dir_left = {.x = -1, .y = 0};
constexpr Vec2<int> dir_up = {.x = 0, .y = -1};
constexpr Vec2<int> dir_down = {.x = 0, .y = 1};

void parse_grid(const std::vector<std::string>& lines, Grid<int>& grid)
{
    for (const auto& line: lines) {
        if (line.size()) {
            std::vector<int> row; 
            for (char c : line) {
                row.push_back(aocio::parse_digit(c).value()); 
            }
            grid.push_row(row);
        }
    }
}

struct State {
    Vec2<int> pos, dir; 
    int straight_cnt; 
    bool operator==(const State&) const = default;
};

template<>
struct std::hash<State> {
    std::size_t operator()(const State& s) const noexcept
    {
        std::size_t h = 0; 
        aocutil::hash_combine(h, s.pos, s.dir, s.straight_cnt);
        return h;
    }
};

std::pair<Vec2<int>, Vec2<int>> get_left_right(const Vec2<int>& dir)
{
    if (dir == dir_right) {
        return {dir_up, dir_down}; 
    } else if (dir == dir_left) {
        return {dir_down, dir_up}; 
    } 
    else if (dir == dir_up) {
        return {dir_left, dir_right}; 
    } else if (dir == dir_down) {
        return {dir_right, dir_left};
    }
    throw std::invalid_argument("get_left_right: Invalid direction");
}

std::vector<State> find_adjacent(const Grid<int>& grid, const State& s, int max_steps)
{   
    std::vector<State> adj; 

    const auto [left_dir, right_dir] = get_left_right(s.dir); 
    Vec2<int> left_pos = s.pos + left_dir;
    Vec2<int> right_pos = s.pos + right_dir;

    if (grid.pos_on_grid(left_pos)) {
        adj.emplace_back(State{.pos = left_pos, .dir = left_dir, .straight_cnt = 0}); 
    }

    if (grid.pos_on_grid(right_pos)) {
        adj.emplace_back(State{.pos = right_pos, .dir = right_dir, .straight_cnt = 0}); 
    }

    if (s.straight_cnt + 1 < max_steps) {
        Vec2<int> straight_pos = s.pos + s.dir; 
        if (grid.pos_on_grid(straight_pos)) {
            adj.emplace_back(State{.pos = straight_pos, .dir = s.dir, .straight_cnt = s.straight_cnt + 1}); 
        }
    }

    return adj;
}

int manhattan_dist(const Vec2<int>& start, const Vec2<int>& target) { // A heuristic is admissible if it never overestimates the actual cost to get to the target. 
    return std::abs(start.x - target.x) + std::abs(start.y - target.y);
}

int no_heuristic(const Vec2<int>& start, const Vec2<int>& target) {
    return 0; 
}

/*
    Implements an A* search algorithm.
    If no_heuristic is used for the heuristic parameter, A* is equivalent to Dijkstra's algorithm. 
    cf. https://en.wikipedia.org/wiki/A*_search_algorithm (last retrieved 2024-06-20)
    cf. https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm (last retrieved 2024-06-20)
*/ 
int find_shortest_path(const Grid<int>& grid, int max_straight_steps = 3, std::function<int(const Vec2<int>&, const Vec2<int>&)> heuristic = no_heuristic)
{
    constexpr int infinity = std::numeric_limits<int>::max();
    const Vec2<int> end_pos = {grid.width() - 1, grid.height() - 1}; 

    PrioQueue<State> queue; 
    std::unordered_map<State, int> cost; // The real cost from the start node to a given node.

    const State start_r = {.pos = {0, 0}, .dir = dir_right}; 
    const State start_d = {.pos = {0, 0}, .dir = dir_down};

    queue.insert(start_r, heuristic(start_r.pos, end_pos)); 
    queue.insert(start_d, heuristic(start_d.pos, end_pos));
    cost.insert({start_r, 0}); 
    cost.insert({start_d, 0});

    int min_cost = infinity; 

    while (!queue.empty()) {
        const State current = queue.extract_min().value();
        const int cost_current = cost.at(current);

        if (current.pos == end_pos) {
            min_cost = std::min(cost_current, min_cost);
            continue;
        }

        for (const State& adj: find_adjacent(grid, current, max_straight_steps)) {
            const int cost_adj_min = cost.contains(adj) ? cost.at(adj) : infinity;
            if (const int cost_new = cost_current + grid[adj.pos]; cost_new <= cost_adj_min) {
                cost.insert_or_assign(adj, cost_new);
                queue.insert_or_update(adj, cost_new + heuristic(adj.pos, end_pos));
            }
        }
    }

    return min_cost;
}

int part_one(const std::vector<std::string>& lines)
{
    Grid<int> grid; 
    parse_grid(lines, grid); 
    return find_shortest_path(grid, 3, no_heuristic);
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
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}