#include <unordered_set>
#include <stack>
#include "../aoclib/aocio.hpp"
#include "../aoclib/grid.hpp"

/*
    Problem: https://adventofcode.com/2023/day/16
  
    Solutions: 
        - Part 1: 6605
        - Part 2: 6766
    Notes:  
        - Implemented a templated grid class with custom random_access_iterators as an exercise. That was fun!
*/

using namespace aocutil; 
using CharGrid = Grid<std::string, char>;

struct Beam {
    Vec2<int> pos, dir;
    bool operator==(const Beam&) const = default;
};

template<>
struct std::hash<Beam> {
    std::size_t operator()(const Beam& b) const noexcept
    {
        std::size_t h1 = std::hash<Vec2<int>>{}(b.pos); 
        std::size_t h2 = std::hash<Vec2<int>>{}(b.dir); 
        return h1 ^ (h2 << 1); // cf. https://en.cppreference.com/w/cpp/utility/hash (last retrieved 2024-06-17)
    }
};

using BeamSet = std::unordered_set<Beam>; 

const std::ostream& operator<<(std::ostream& os, const BeamSet& bs)
{
    if (bs.size()) {
        return os << "#";
    } else {
        return os << ".";
    }
}

constexpr Vec2<int> dir_right = {.x = 1, .y = 0};
constexpr Vec2<int> dir_left = {.x = -1, .y = 0};
constexpr Vec2<int> dir_up = {.x = 0, .y = -1};
constexpr Vec2<int> dir_down = {.x = 0, .y = 1};

void parse_grid(const std::vector<std::string>& lines, CharGrid& grid)
{
    for (const auto& line: lines) {
        if (line.size()) {
            grid.push_row(line);
        }
    }
}

int calculate_energized(const CharGrid& grid, Beam start_beam)
{
    Grid<std::vector<BeamSet>, BeamSet> energised_grid; 
    for (int y = 0; y < grid.height(); ++y) {
        std::vector<BeamSet> row(grid.width(), BeamSet()); 
        energised_grid.push_row(row);
    }

    std::stack<Beam> beams; // We follow the beam depth-first since we use a stack here (breadth-first if we used a queue).
    beams.push(start_beam);

    while (beams.size()) {
        Beam b = beams.top();
        beams.pop();
        auto sym = grid.try_get(b.pos); 
        
        if (!sym) { // The beam left the grid. 
            continue; 
        }

        if (energised_grid.at(b.pos).contains(b)) { // The tile was already visited by the same beam. 
            continue; 
        } else {
            energised_grid.at(b.pos).insert(b);
        }

        switch (sym.value())
        {
        case '.': {
            Beam new_b = {.dir = b.dir, .pos = b.pos + b.dir};
            beams.push(new_b);
            break;
        }

        case '/':
        case '\\': {
            const bool is_mirror_right = sym.value() == '/';
            if (b.dir == dir_right) { 
                b.dir = is_mirror_right ? dir_up : dir_down; 
            } else if (b.dir == dir_left) {
                b.dir = is_mirror_right ? dir_down : dir_up; 
            } else if (b.dir == dir_up) {
                b.dir = is_mirror_right ? dir_right : dir_left; 
            } else if (b.dir == dir_down) {
                b.dir = is_mirror_right ? dir_left : dir_right; 
            }
            b.pos = b.pos + b.dir;
            beams.push(b);
            break; 
        }

        case '|': {
            if (b.dir == dir_down || b.dir == dir_up) {
                b.pos = b.pos + b.dir;
                beams.push(b);
            } else {
                Beam b_up = {.pos = b.pos, .dir = dir_up};
                Beam b_down = {.pos = b.pos, .dir = dir_down};
                beams.push(b_up);
                beams.push(b_down);
            }
            break;
        }

        case '-': {
            if (b.dir == dir_left || b.dir == dir_right) {
                b.pos = b.pos + b.dir;
                beams.push(b);
            } else {
                Beam b_left = {.pos = b.pos, .dir = dir_left};
                Beam b_right = {.pos = b.pos, .dir = dir_right};
                beams.push(b_left);
                beams.push(b_right);
            }
            break;
        }

        default:
            assert(false);
            break;
        }
    }
    // std::cout << energised_grid << "\n";
    return std::count_if(energised_grid.cbegin(), energised_grid.cend(), [](const BeamSet& bs) {return bs.size() > 0;});
}

int part_one(const std::vector<std::string>& lines)
{
    CharGrid grid; 
    parse_grid(lines, grid);
    return calculate_energized(grid, Beam{.pos={0, 0}, .dir = dir_right});
}

int part_two(const std::vector<std::string>& lines)
{
    CharGrid grid;
    parse_grid(lines, grid);

    std::vector<Beam> start_beams; 
    for (int x = 0; x < grid.width(); ++x) { // Top and bottom edge. 
        Beam b_top = {.pos = Vec2<int>{x, 0}, .dir = dir_down};
        Beam b_bottom = {.pos = Vec2<int>{x, grid.height() - 1}, .dir = dir_up};
        start_beams.push_back(b_top); 
        start_beams.push_back(b_bottom);
    }
    for (int y = 0; y < grid.height(); ++y) { // Left and right edge.
        Beam b_left = {.pos = Vec2<int>{0, y}, .dir = dir_right};
        Beam b_right = {.pos = Vec2<int>{grid.width() - 1, y}, .dir = dir_left};
        start_beams.push_back(b_left); 
        start_beams.push_back(b_right);
    }

    int max_energised = 0; 
    for (const Beam& beam : start_beams) {
        max_energised = std::max(calculate_energized(grid, beam), max_energised);
    }
    return max_energised; 
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