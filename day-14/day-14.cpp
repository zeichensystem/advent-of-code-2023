#include <unordered_map>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/14
  
    Solutions: 
        - Part 1: 107053
        - Part 2: 88371
    Notes:  
        - This was really fun!
*/

struct Vec2 
{
    int x, y; 

    Vec2 operator+(const Vec2& b) const
    {
        return Vec2 {
            .x = x + b.x,
            .y = y + b.y
        };
    }
};

class Grid 
{
private: 
    std::vector<std::string> rows; 
    int width, height; 

public:
    Grid(const std::vector<std::string>& lines)
    {
        width = std::ssize(lines.at(0));
        height = std::ssize(lines);
        for (auto const& line: lines) {
            if (!line.size() || line.find_first_not_of(" \t") == std::string::npos) {
                height -= 1; 
            } else {
                width = std::ssize(line);
                rows.push_back(line);
            }
        }
        assert(width > 0 && height > 0); 
    }

    std::optional<char> get(const Vec2& pos) const
    {
        if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) {
            return {};
        } 
        return rows.at(pos.y).at(pos.x);
    }

    void set(const Vec2& pos, char sym) 
    {
        rows.at(pos.y).at(pos.x) = sym;
    }

private: 
    void calc_start_end(const Vec2&dir, int& start_x, int& end_x, int& start_y, int& end_y) const
    {
        assert(dir.x == 0 || dir.y == 0);
        if (dir.y == -1) { // North
            start_y = 0; 
            start_x = 0; 
        } else if (dir.y == 1) { // South
            start_y = height - 1; 
            start_x = 0; 
        } else if (dir.x == -1) { // West
            start_x = 0; 
            start_y = 0; 
        } else if (dir.x == 1) { // East
            start_x = width - 1; 
            start_y = 0;
        }
        end_y = start_y == 0 ? height - 1 : 0; 
        end_x = start_x == 0 ? width - 1 : 0;
    }

    auto get_xynext_fun(int start, int end) const
    {
        auto xy_next = [start, end](int& xy) {
            if (start < end) {
                ++xy; 
            } else {
                --xy; 
            }
        };
        return xy_next;
    }

    auto get_xycmp_fun(int start, int end) const
    {
        auto xy_cmp = [start, end](int xy) -> bool {
            if (start < end) {
                return xy <= end; 
            } else {
                return xy >= end;
            }
        };
        return xy_cmp; 
    }

public: 
    void move_rocks(const Vec2& dir)
    {
        assert(dir.x == 0 || dir.y == 0); 
        int start_x, end_x, start_y, end_y;
        calc_start_end(dir, start_x, end_x, start_y, end_y);
        const auto y_next = get_xynext_fun(start_y, end_y);
        const auto x_next = get_xynext_fun(start_x, end_x);
        const auto y_cmp = get_xycmp_fun(start_y, end_y); 
        const auto x_cmp = get_xycmp_fun(start_x, end_x); 

        for (int y = start_y; y_cmp(y); y_next(y)) {
            for (int x = start_x; x_cmp(x); x_next(x)) {
                Vec2 pos = {x, y}; 
                auto sym = get(pos); 
                assert(sym.has_value());
                if (sym.value() == 'O') {
                    while (get(pos + dir).has_value() && get(pos + dir).value() == '.') {
                        Vec2 new_pos = pos + dir; 
                        set(pos, '.');
                        set(new_pos, 'O');
                        pos = new_pos;
                    }
                }
            }
        }
    }

    void move_rocks_cycle()
    {
        move_rocks(Vec2{0, -1}); // North
        move_rocks(Vec2{-1, 0}); // West
        move_rocks(Vec2{0, 1});  // South
        move_rocks(Vec2{1, 0});  // East
    }

    int calc_load(const Vec2& dir) const
    {
        assert(dir.x == 0 || dir.y == 0); 
        int start_x, end_x, start_y, end_y;
        calc_start_end(dir, start_x, end_x, start_y, end_y);
        const auto y_next = get_xynext_fun(start_y, end_y);
        const auto x_next = get_xynext_fun(start_x, end_x);
        const auto y_cmp = get_xycmp_fun(start_y, end_y); 
        const auto x_cmp = get_xycmp_fun(start_x, end_x);

        int total_load = 0; 
        if (dir.x == 0) { // Count 'O' row by row. 
            int weight_fac = height; 
            for (int y = start_y; y_cmp(y); y_next(y), --weight_fac) {
                int row_cnt = 0; 
                for (int x = start_x; x_cmp(x); x_next(x)) {
                    Vec2 pos = {x, y}; 
                    if (get(pos).value() == 'O') {
                        ++row_cnt;
                    }
                }
                assert(weight_fac > 0);
                total_load += row_cnt * weight_fac; 
            }
        } else { // Count 'O' column by column.
            int weight_fac = width; 
            for (int x = start_x; x_cmp(x); x_next(x), --weight_fac) {
                int col_cnt = 0; 
                for (int y = start_y; y_cmp(y); y_next(y)) {
                    Vec2 pos = {x, y}; 
                    if (get(pos).value() == 'O') {
                        ++col_cnt;
                    }
                }
                assert(weight_fac > 0);
                total_load += col_cnt * weight_fac; 
            }
        }
        return total_load;
    }

    bool operator==(const Grid& other) const 
    {
        if (width != other.width || height != other.height) {
            return false; 
        }
        for (int y = 0; y < height; ++y) {
            if (rows.at(y) != other.rows.at(y)) {
                return false;
            }
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid& g)
    {
        for (const auto& line : g.rows) {
            os << line << "\n";
        }
        return os;
    }

    friend std::hash<Grid>; 
};

template<>
struct std::hash<Grid>
{
    std::size_t operator()(const Grid& g) const noexcept
    {   
        std::string str_repr = "";
        for (const auto& row : g.rows) {
            str_repr.append(row) + "\n";
        }
        std::size_t h1 = std::hash<std::string>{}(str_repr);
        return h1;
    }
};

int part_one(const std::vector<std::string>& lines)
{
    const Vec2 north {0, -1}; 
    Grid grid {lines}; 
    grid.move_rocks(north); 
    return grid.calc_load(north); 
}

int part_two(const std::vector<std::string>& lines)
{
    Grid grid {lines}; 
    std::unordered_map<Grid, int> cache; 
    constexpr int cycle_max = 1000'000'000;

    for (int cycle = 1; cycle <= cycle_max; ++cycle) {
        grid.move_rocks_cycle();
        if (!cache.contains(grid)) {
            cache.insert({grid, cycle}); 
        } else {
            int remaining_cycles = cycle_max - cycle; 
            int period = cycle - cache.at(grid);
            assert(period >= 0);
            if (period == 0) {
                break;
            }
            int rem = remaining_cycles % period; 
            while (rem-- > 0) {
                grid.move_rocks_cycle();
            }
            break;
        }
    }

    const Vec2 north {0, -1}; 
    return grid.calc_load(north);
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