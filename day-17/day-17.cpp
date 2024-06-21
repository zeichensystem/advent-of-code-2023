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

        - Above does not apply to the optimised solution, because it does without unordered_maps. 
*/

using aocutil::Vec2; 
using aocutil::Grid; 

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

constexpr int infinity = std::numeric_limits<int>::max();

struct State {
    Vec2<int> pos, dir; 
    int straight_cnt; 

    int cost = infinity;
    bool visited = false;

    auto operator<=>(const State& other) const {return cost - other.cost;};
    bool operator==(const State& other) const {return pos == other.pos && dir == other.dir && straight_cnt == other.straight_cnt;};
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


constexpr int max_straight_steps_limit = 11; 
constexpr int num_directions = 4; 
constexpr int max_states_per_cell = num_directions * max_straight_steps_limit; 

int calc_state_idx(const State& s)
{
    int idx = 0; 
    if (s.dir == dir_up) {
        idx = 1 * max_straight_steps_limit; 
    } else if (s.dir == dir_right) {
        idx = 2 * max_straight_steps_limit; 
    } else if (s.dir == dir_down) {
        idx = 3 * max_straight_steps_limit;
    }
    idx += s.straight_cnt; 
    assert(idx < max_states_per_cell); 
    return idx; 
}

struct StateCell 
{
    std::array<State, max_states_per_cell> states; 
    std::size_t size = 0; 

    using StateArrIter = typename decltype(states)::iterator; 

    StateArrIter insert(const State& s)
    {
        assert(size < states.size()); 
        states[size] = s; 
        return states.begin() + (size++);
    }

    std::optional<StateArrIter> find(const State& s)
    {
        auto end_it = states.begin() + size; 
        auto it = std::find(states.begin(), end_it, s); 
        if (it != end_it) {
            return it; 
        } else {
            return {}; 
        }
    }
};

std::vector<StateCell::StateArrIter> find_adjacent(const Grid<int>& grid, Grid<StateCell>& state_grid, const State& s, int min_steps, int max_steps)
{   
    std::vector<StateCell::StateArrIter> adj; 

    if (s.straight_cnt >= min_steps) {
        const auto [left_dir, right_dir] = get_left_right(s.dir); 
        Vec2<int> left_pos = s.pos + left_dir;
        Vec2<int> right_pos = s.pos + right_dir;

        if (grid.pos_on_grid(left_pos)) {
            State state {.pos = left_pos, .dir = left_dir, .straight_cnt = 1, .cost = infinity, .visited = false};
            auto found = state_grid.at(state.pos).find(state);
            if (found) {
                adj.push_back(found.value());
            } else {
                adj.push_back(state_grid.at(state.pos).insert(state));
            }
        }

        if (grid.pos_on_grid(right_pos)) {
            State state {.pos = right_pos, .dir = right_dir, .straight_cnt = 1, .cost = infinity, .visited = false}; 
            auto found = state_grid.at(state.pos).find(state);
            if (found) {
                adj.push_back(found.value());
            } else {
                adj.push_back(state_grid.at(state.pos).insert(state));
            }
        }
    }

    if (s.straight_cnt < max_steps) {
        Vec2<int> straight_pos = s.pos + s.dir; 
        if (grid.pos_on_grid(straight_pos)) {
            State state {.pos = straight_pos, .dir = s.dir, .straight_cnt = s.straight_cnt + 1, .cost = infinity, .visited = false}; 
            auto found = state_grid.at(state.pos).find(state);
            if (found) {
                adj.push_back(found.value());
            } else {
                adj.push_back(state_grid.at(state.pos).insert(state));
            }
        }
    }

    return adj;
}

int find_shortest_path(const Grid<int>& grid, int min_straight_steps = 0, int max_straight_steps = 3)
{
    const Vec2<int> end_pos = {grid.width() - 1, grid.height() - 1}; 

    State start_r = {.pos = {0, 0}, .dir = dir_right, .straight_cnt = 0, .cost = 0, .visited = false}; 
    State start_d = {.pos = {0, 0}, .dir = dir_down, .straight_cnt = 0, .cost = 0, .visited = false};

    std::priority_queue<State, std::vector<State>, std::greater<State>> queue; 
    queue.push(start_r);
    queue.push(start_d);

    Grid<StateCell> state_grid;
    for (int y = 0; y < grid.height(); ++y) {
        state_grid.push_row(std::vector<StateCell>(grid.width(), StateCell{.size = 0}));
    }

    state_grid.at(start_r.pos).insert(start_r);
    state_grid.at(start_d.pos).insert(start_d);

    int min_cost = infinity; 
    
    while (!queue.empty()) {
        const State current = queue.top();
        queue.pop();

        auto current_ref = state_grid.at(current.pos).find(current);
        assert(current_ref); 

        if (current_ref.value()->visited) {
            continue;
        } 
        current_ref.value()->visited = true;

        if (current.pos == end_pos && current.straight_cnt >= min_straight_steps) {
            std::cout << "q: " << queue.size() << "\n";
            return current.cost;
        }
    

        for (StateCell::StateArrIter adj: find_adjacent(grid, state_grid, current, min_straight_steps, max_straight_steps)) {
            if (adj->visited) {
                continue;
            }
            if (int new_cost = current.cost + grid.at(adj->pos); new_cost <= adj->cost) {
                adj->cost = new_cost;
                queue.push(*adj);
            }
        }
    }

    return min_cost;
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