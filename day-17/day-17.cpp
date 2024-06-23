#include <queue>
#include <array>
#include "../aoclib/aocio.hpp"
#include "../aoclib/grid.hpp"

/*
    Problem: https://adventofcode.com/2023/day/17
  
    Solutions: 
        - Part 1: 907
        - Part 2: 1057
    Notes:  
        - Implemented a priority queue for aocutil
        - Manhattan distance does not seem to be a good heuristic here and runs even slower. (Or maybe I implemented something wrong...) 
        - Found out the way I combined hashes (i.e. h_combined = h1 ^ (h2 << 1), cf. the example code from  https://en.cppreference.com/w/cpp/utility/hash)
          was really bad. Using a hash_combine function I found produces better hashes and therefore makes code using maps etc. way faster.
          When using hash_combine, part 1 runs in ~0.5 seconds instead of ~6 seconds.
        - Part 2: Harder than expected; off-by-one-error, the problem.

        - Above does not apply to the optimised solution, because it does without unordered_maps and uses
          std::priority_queue
*/

using aocutil::Vec2; 
using aocutil::Direction; 
using aocutil::dir_to_vec2; 
using aocutil::dir_get_left_right; 
using aocutil::Grid; 

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

constexpr int infinity = std::numeric_limits<int>::max();

struct State {
    Vec2<int> pos; 
    Direction dir; 
    int straight_cnt; 
};

constexpr int max_straight_steps_limit = 10; 
constexpr int num_directions = 4; 
constexpr int max_states_per_cell = num_directions * (max_straight_steps_limit + 1); 

struct CostGridCell 
{
private: 
    std::array<int, max_states_per_cell> id_to_cost;

    constexpr int calc_state_idx(const State& s) const
    {
        int idx = 0; 
        if (s.dir == Direction::Up) {
            idx = 1 * max_straight_steps_limit; 
        } else if (s.dir == Direction::Right) {
            idx = 2 * max_straight_steps_limit; 
        } else if (s.dir == Direction::Down) {
            idx = 3 * max_straight_steps_limit;
        } else {
            assert(s.dir == Direction::Left); 
        }
        idx += s.straight_cnt; 
        assert(idx >= 0); 
        assert(idx < std::ssize(id_to_cost));
        return idx; 
    }

public:
    CostGridCell() 
    {
        for (auto& cost : id_to_cost) {
            cost = infinity;  
        }
    }

    void save_cost(const State& s, int cost)
    {
        int idx = calc_state_idx(s);
        id_to_cost.at(idx) = cost; 
    }

    int get_cost(const State& s) const
    {
        int idx = calc_state_idx(s); 
        return id_to_cost.at(idx); 
    }
};

std::array<State, 3> find_adjacent(const Grid<int>& grid, const State& s, int min_steps, int max_steps, int &size)
{   
    std::array<State, 3> adj; 
    size = 0; 

    if (s.straight_cnt >= min_steps) {
        const auto [left_dir, right_dir] = dir_get_left_right(s.dir); 
        Vec2<int> left_pos = s.pos + dir_to_vec2<int>(left_dir);
        Vec2<int> right_pos = s.pos + dir_to_vec2<int>(right_dir);

        if (grid.pos_on_grid(left_pos)) {
            assert(size < std::ssize(adj));
            adj.at(size++) = State{.pos = left_pos, .dir = left_dir, .straight_cnt = 1}; 
        }

        if (grid.pos_on_grid(right_pos)) {
            assert(size < std::ssize(adj));
            adj.at(size++) = (State{.pos = right_pos, .dir = right_dir, .straight_cnt = 1});
        }
    }

    if (s.straight_cnt < max_steps) {
        Vec2<int> straight_pos = s.pos + dir_to_vec2<int>(s.dir); 
        if (grid.pos_on_grid(straight_pos)) {
            assert(size < std::ssize(adj));
            adj.at(size++) = (State{.pos = straight_pos, .dir = s.dir, .straight_cnt = s.straight_cnt + 1}); 
        }
    }

    return adj;
}

int heuristic(const Vec2<int>& pos, const Vec2<int>& end_pos)
{
    return 0; 
    // return std::abs(end_pos.x - pos.x) + std::abs(end_pos.y - pos.x);
}

/* 
    Dijkstra implemented using array-based priority queues without "decrease-priority" functionality: 
    cf. https://en.wikipedia.org/wiki/Dijkstra's_algorithm#Using_a_priority_queue 
        https://cs.stackexchange.com/questions/118388/dijkstra-without-decrease-key (last retrieved 2024-06-22)
*/
int find_shortest_path(const Grid<int>& grid, int min_straight_steps = 0, int max_straight_steps = 3)
{
    if (max_straight_steps > max_straight_steps_limit) {
        throw std::invalid_argument("find_shortest_path: max_straight_steps > max_straight_steps_limit, increase the limit");
    }

    const Vec2<int> end_pos = {grid.width() - 1, grid.height() - 1}; 

    State start_r = {.pos = {0, 0}, .dir = Direction::Right, .straight_cnt = 0}; 
    State start_d = {.pos = {0, 0}, .dir = Direction::Down, .straight_cnt = 0};

    using CostStatePair = typename std::pair<int, State>; 
    const auto cmp_prio = [](const CostStatePair& a, const CostStatePair& b) -> bool {
        return a.first > b.first; 
    };
    std::priority_queue<CostStatePair, std::vector<CostStatePair>, decltype(cmp_prio)> queue(cmp_prio); 
    queue.push(CostStatePair{heuristic(start_r.pos, end_pos), start_r});
    queue.push(CostStatePair{heuristic(start_d.pos, end_pos), start_d});

    Grid<CostGridCell> cost_grid;
    for (int y = 0; y < grid.height(); ++y) {
        cost_grid.push_row(std::vector<CostGridCell>(grid.width(), CostGridCell()));
    }
    cost_grid.at(start_r.pos).save_cost(start_r, 0);
    cost_grid.at(start_d.pos).save_cost(start_d, 0);
    
    while (!queue.empty()) {
        const auto [priority, current] = queue.top();
        queue.pop();
        
        int current_cost = cost_grid.at(current.pos).get_cost(current); 

        if (current.pos == end_pos && current.straight_cnt >= min_straight_steps) {
            return current_cost;
        }
        
        if (priority != current_cost + heuristic(current.pos, end_pos)) { // State was already in queue. 
            assert(priority > current_cost);
            continue;
        }
        
        int num_neighbors = 0; 
        auto neighbors = find_adjacent(grid, current, min_straight_steps, max_straight_steps, num_neighbors);
        for (int i = 0; i < num_neighbors; ++i) {
            State adj = neighbors[i]; 
            int adj_cost = cost_grid.at(adj.pos).get_cost(adj);
            if (int new_cost = current_cost + grid.at(adj.pos); new_cost < adj_cost) { // Must not be new_cost <= adj_cost
                cost_grid.at(adj.pos).save_cost(adj, new_cost);
                assert(new_cost != infinity);
                queue.emplace(new_cost + heuristic(adj.pos, end_pos), adj);
            }
        }
    }
    return infinity;
}

int part_one(const std::vector<std::string>& lines)
{
    Grid<int> grid; 
    parse_grid(lines, grid); 
    return find_shortest_path(grid, 0, 3);
}

int part_two(const std::vector<std::string>& lines)
{
    Grid<int> grid;
    parse_grid(lines, grid);
    return find_shortest_path(grid, 4, 10);
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