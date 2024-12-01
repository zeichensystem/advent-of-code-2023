#include <unordered_set>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/11
  
    Solutions: 
        - Part 1: 9608724
        - Part 2: 904633799472
    Notes: 
        - I initially solved part 1 the stupid way by inserting empty rows/columns, but 
          the "smart" way turned out to be easier for both parts. 
*/

class Grid 
{
public:
    struct Galaxy {
        int64_t x, y; 
    }; 

    static constexpr char galaxy_sym = '#'; 
    static constexpr char empty_sym = '.'; 
    
    int width = 0, height = 0; 
    std::vector<Galaxy> galaxies; 
    std::unordered_set<int64_t> empty_rows, empty_cols; 

    Grid(const std::vector<std::string>& lines, int expand_size = 2)
    {
        int64_t line_row = 0; 
        for (std::string line : lines) {
            data.push_back(line);
            bool empty_row = line.find(Grid::galaxy_sym) == std::string::npos; 
            if (empty_row) {
                empty_rows.insert(line_row); 
            }
            ++line_row; 
        }
        height = data.size(); 
        width = data.at(0).size();

        for (int col = 0; col < width; ++col) {
            bool col_empty = true;
            for (int row = 0; row < height; ++row) {
                if (get(col, row) == Grid::galaxy_sym) {
                    col_empty = false; 
                    break; 
                }
            }
            if (col_empty) {
                empty_cols.insert(col); 
            }
        }

        int64_t row_delta = 0; 
        for (int row = 0; row < height; ++row) {
            if (empty_rows.contains(row)) {
                row_delta += expand_size - 1; 
            }
            int64_t col_delta = 0; 
            for (int col = 0; col < width; ++col) {
                if (empty_cols.contains(col)) {
                    col_delta += expand_size - 1;
                }
                if (get(col, row) == Grid::galaxy_sym) {
                    galaxies.push_back(Galaxy{.x = col + col_delta, .y = row + row_delta});
                }
            }
        }
    }

    char get(int x, int y) const
    {
        char sym = data.at(y).at(x); 
        assert(sym == Grid::galaxy_sym || sym == Grid::empty_sym); 
        return sym;
    }

    friend std::ostream& operator<<(std::ostream &os, const Grid &grid) {
        for (const auto &row : grid.data) {
            for (char c : row) {
                os << c;
            }
            os << "\n";
        }
        return os; 
    }

private: 
    std::vector<std::string> data; 
};

int64_t part_one(const std::vector<std::string>& lines, int64_t expand_size = 2)
{
    Grid galaxy_grid = Grid(lines, expand_size); 

    // There are n * (n - 1) / 2 edges in a complete graph. 
    size_t num_pairs = galaxy_grid.galaxies.size() * (galaxy_grid.galaxies.size() - 1) / 2; 
    int64_t galaxy_pair_distances = 0; 
    size_t pairs_processed = 0; 
    for (size_t i = 0; i < galaxy_grid.galaxies.size(); ++i) {
        for (size_t j = i + 1; j < galaxy_grid.galaxies.size(); ++j) {
            const auto& a = galaxy_grid.galaxies.at(i); 
            const auto& b = galaxy_grid.galaxies.at(j); 
            int64_t manhattan_dist = std::abs(a.x - b.x) + std::abs(a.y - b.y); 
            galaxy_pair_distances += manhattan_dist; 
            ++pairs_processed; 
        }
    }
    assert(pairs_processed == num_pairs); 
    return galaxy_pair_distances;
}

int64_t part_two(const std::vector<std::string>& lines)
{
    return part_one(lines, 1000'000);
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
        int64_t p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        int64_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}