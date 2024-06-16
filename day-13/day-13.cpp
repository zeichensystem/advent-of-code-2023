#include <unordered_map>
#include <unordered_set>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/13
  
    Solutions: 
        - Part 1: 30705
        - Part 2: 44615
    Notes: 
        - Part 2 was pretty easy because I had already solved part 1
          with a hashtable counting how many times each mirror start position
          occurs in each pattern: 
          For part 1, I check if the count is equal to the pattern's 
          number of rows/cols, for part 2, I check if the count is 
          equal to the pattern's number of rows/cols minus one. 
*/

enum class Axis {Vertical, Horizontal};

struct Pattern 
{
    std::vector<std::string> lines; 

    std::optional<int> reflection(Axis axis, bool find_smudge = false) const
    {
        std::unordered_map<int, int> mirror_starts;

        auto line_mirror_start = [&mirror_starts](const std::string& line) {
            for (int left = 0; left < std::ssize(line) - 1; ++left) {
                int max_delta = std::min(left+1, std::abs((int)std::ssize(line) - 1 - left));
                bool is_mirrored = true; 
                for (int d = 1; d <= max_delta; ++d) {
                    char left_sym = line.at(left + 1 - d); 
                    char right_sym = line.at(left + d);
                    if (left_sym != right_sym) {
                        is_mirrored = false; 
                        break;
                    }
                }
                if (is_mirrored) {
                    int col = left + 1; 
                    if (mirror_starts.contains(col)) {
                        mirror_starts.at(col) += 1; 
                    } else {
                        mirror_starts.insert({col, 1});
                    }
                }
            }
        }; 

        switch (axis) {
        case Axis::Vertical:
            for (const auto& line: lines) {
                line_mirror_start(line); 
            }
            for (const auto& [mirror_start, cnt] : mirror_starts) {
                if (find_smudge && cnt == std::ssize(lines) - 1) {
                    return mirror_start; 
                } else if (!find_smudge && cnt == std::ssize(lines)) {
                    return mirror_start;
                }
            }
            break;

        case Axis::Horizontal:
            for (int col = 0; col < std::ssize(lines.at(0)); ++col) {
                std::string column = "";
                for (int row = 0; row < std::ssize(lines); ++row) {
                    column.push_back(lines.at(row).at(col)); 
                }
                line_mirror_start(column); 
            }
            for (const auto& [mirror_start, cnt] : mirror_starts) {
                if (find_smudge && cnt == std::ssize(lines.at(0)) - 1) {
                    return mirror_start;
                } else if (!find_smudge && cnt == std::ssize(lines.at(0))) {
                    return mirror_start; 
                }
            }
            break;

        default:
            throw "Not an axis";
            break;
        }
        return {};
    }

    friend std::ostream& operator<<(std::ostream& os, const Pattern& p)
    {
        for (const auto& line : p.lines) {
            for (char c : line) {
                os << c;
            }
            os << "\n";
        }
        return os;
    }
}; 

void parse_patterns(const std::vector<std::string>& lines, std::vector<Pattern>& result)
{
    Pattern pat; 
    for (const auto& line: lines) {
        if (line.size() == 0) {
            if (pat.lines.size() != 0) {
                result.push_back(pat);
            }
            pat = {}; 
        } else {
            pat.lines.push_back(line);
        }
    }
    if (pat.lines.size() != 0) {
        result.push_back(pat);
    }
}

int64_t part_one(const std::vector<std::string>& lines, bool find_smudge = false)
{
    std::vector<Pattern> patterns;
    parse_patterns(lines, patterns); 

    int64_t cols = 0, rows = 0; 
    for (const auto& pat : patterns) {
        auto mirror_vert = pat.reflection(Axis::Vertical, find_smudge ); 
        if (mirror_vert.has_value()) {
            cols += mirror_vert.value(); 
        }
        auto mirror_horiz = pat.reflection(Axis::Horizontal, find_smudge); 
        if (mirror_horiz.has_value()) {
            rows += mirror_horiz.value();
        }
        assert(!(mirror_vert.has_value() && mirror_horiz.has_value()));
        assert((mirror_vert.has_value() || mirror_horiz.has_value()));
    }
    return cols + 100 * rows;
}

int64_t part_two(const std::vector<std::string>& lines)
{
    return part_one(lines, true);
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