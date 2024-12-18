#include <algorithm>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/3

    Solutions:
        - Part 1: 560670
        - Part 2: 91622824

    Notes:
        - Nice C++20 feature: default struct comparison operators
        - Part 2: - Just one index (either begin_idx or end_idx) would be sufficient for NumInfo
                  - Could use an unordered_set instead of a vector for nums (in get_gear_ratio), but n <= 8 so it doesn't really matter
                  - Fix lambda: capture lines as reference to avoid expensive copying (I thought it was not necessary because lines is already a const ref...)
                  - My part one had a bug, but I was "lucky" with my puzzle input so I passed by sheer luck..
                  - My part two also had a bug; and I was also "lucky" with my puzzle input again and didn't catch it because I passed...  
                    I didn't consider I have to save the row in NumInfo, without that two different numbers might compare equal when they aren't (which can lead to misclassification of gears)
        - Use std::ssize instead of casting to int where necessary
        - Use std::isdigit
        - I wonder which puzzle will be the first one to overflow int or int64_t... 
          (Update from the future: It's day 5 part 1)

*/

static int digit_char_to_int(const char c)
{
    if (!std::isdigit(c)) {
        throw "char is not a digit";
    }
    return c - '0'; 
}

static bool idx_in_range(int num_rows, int num_cols, int row, int col)
{
    bool in_row = row >= 0 && row < num_rows; 
    bool in_col = col >= 0 && col < num_cols;
    return in_row && in_col; 
}

static bool adjacent_to_symbol(const std::vector<std::string>& lines, int row, int col)
{
    if (row < 0 || row >= std::ssize(lines)) {
        throw "row-index out of range"; 
    }
    if (col < 0 || col >= std::ssize(lines.at(row))) {
        throw "col-index out of range";
    }
    const int num_rows = lines.size();
    const int num_cols = lines.at(row).size(); 

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if ((dx == 0 && dy == 0) || !idx_in_range(num_rows, num_cols, row + dy, col + dx)) {
                continue;
            } else if (lines.at(row + dy).at(col + dx) != '.' && !std::isdigit(lines.at(row + dy).at(col + dx)) ) {
                return true; 
            }
        }
    }
    return false; 
}

int part_one(const std::vector<std::string>& lines)
{
    int part_sum = 0; 
    for (int row = 0; row < std::ssize(lines); ++row) {
        int num_scale = 1;
        int current_num = 0; 
        bool num_is_adj = false; 
        for (int col = std::ssize(lines.at(row)) - 1; col >= 0; --col) { // From right to left.
            char c = lines.at(row).at(col); 
            if (std::isdigit(c)) {
                num_is_adj = adjacent_to_symbol(lines, row, col) || num_is_adj; 
                current_num += num_scale * digit_char_to_int(c); 
                num_scale *= 10; 
                if (col == 0 && num_is_adj) { // NOTE: Forgot the && num_is_adj but my solution worked for my puzzle input, so I didn't notice... 
                    part_sum += current_num; 
                }
            } else {
                if (num_is_adj) {
                    part_sum += current_num; 
                }
                num_scale = 1; 
                current_num = 0; 
                num_is_adj = false; 
            }
        }
    }
    return part_sum;
}

static int get_gear_ratio(const std::vector<std::string>& lines, int row, int col)
{
    if (row < 0 || row >= std::ssize(lines)) {
        throw "row-index out of range"; 
    }
    if (col < 0 || col >=  std::ssize(lines.at(row))) {
        throw "col-index out of range";
    }
    const int num_rows = lines.size();
    const int num_cols = lines.at(row).size();

    struct NumInfo {
        int num, col_begin_idx, row_idx; 
        bool operator==(const NumInfo&) const = default; // C++20
    };

    auto parse_num = [&lines, num_cols](int row, int col) -> NumInfo { // NOTE: reference-capture lines with &lines to avoid copying (yes, despite lines already being a const ref). 
        int begin_idx = col;
        while (begin_idx + 1 < num_cols && std::isdigit(lines.at(row).at(begin_idx + 1))) {
            ++begin_idx;
        }
        int num = digit_char_to_int(lines.at(row).at(begin_idx)); 
        int num_power = 10;
        int end_idx = begin_idx; 
        while (end_idx - 1 >= 0 && std::isdigit(lines.at(row).at(end_idx - 1))) {
            num += num_power * digit_char_to_int(lines.at(row).at(end_idx - 1));
            num_power *= 10;
            --end_idx;
        }
        assert(end_idx <= begin_idx);
        return NumInfo {.num=num, .col_begin_idx=begin_idx, .row_idx=row}; // NOTE: I have to save the row_idx, otherwise two different numbers might compare as equal when they aren't. 
    }; 

    std::vector<NumInfo> nums;  
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if ((dx == 0 && dy == 0) || !idx_in_range(num_rows, num_cols, row + dy, col + dx)) {
                continue;
            }   
            if (nums.size() > 2) { // Can't be a gear.
                return 0;
            }
            if (std::isdigit(lines.at(row + dy).at(col + dx))) {
                NumInfo num = parse_num(row + dy, col + dx);
                if (std::find(nums.begin(), nums.end(), num) == nums.end()) { // Avoid duplicates. 
                    nums.push_back(num);
                }
            }
        }
    }

    if (nums.size() == 2) { // It is in fact a gear. 
        return nums.at(0).num * nums.at(1).num; 
    } else {
        return 0;
    }
}

int64_t part_two(const std::vector<std::string>& lines)
{
    int64_t gear_ratio_sum = 0; 
    for (int row = 0; row < std::ssize(lines); ++row) {
        for (int col = std::ssize(lines.at(row)) - 1; col >= 0; --col) { // From right to left.
            char c = lines.at(row).at(col); 
            if (c == '*') {
                gear_ratio_sum += get_gear_ratio(lines, row, col);
            }
        }
    }
    return gear_ratio_sum;
}

int main()
{
    aocio::print_day();
    std::vector<std::string> lines;
    std::string_view fname = AOC_INPUT_PATH;
    bool file_loaded = aocio::file_getlines(fname, lines);
    if (!file_loaded) {
        std::cerr << "Error: " << "File '" << fname << "' not found\n";
        return -1;
    }

    try {
        int p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        int64_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return -1;
    }

    return 0;
}