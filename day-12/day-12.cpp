#include <unordered_set>
#include <chrono>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/12
  
    Solutions: 
        - Part 1: 8075
        - Part 2: 
    Notes: 
        - Implemented both dumb brute-force and slightly smarter back-tracking for Part 1. 
          Backtracking is about 30 times faster (in release mode, in debug mode with asan, brute force is even slower).
          
        - I first implemented the backtracking solution, which did not work. Then I implemented the brute force
          solution, but it also did not yield the right solution. That way I found out my is_valid_arrangement function
          was broken, and not my backtracking approach. Funnily enough, the validation function worked for the example inputs,
          but failed on the real input. 
          The backtracking approach also worked for the example inputs with the broken validation function, 
          but the brute-force approach did't work for either input with the broken validation function. 
*/

struct SpringRecord {
    std::string condition; 
    std::vector<int>damaged_groups; 

    friend std::ostream& operator<<(std::ostream& os, const SpringRecord& sr) 
    {
        os << sr.condition << " "; 
        for (int n : sr.damaged_groups) 
            os << n << ",";
        return os << "\n";
    }
};

void parse_spring_records(const std::vector<std::string>& lines, std::vector<SpringRecord> &result)
{
    for (const auto &line : lines) {
        SpringRecord sr; 
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t", "", toks);
        assert(toks.size() == 2); 
        sr.condition = toks.at(0);

        std::vector<std::string> str_nums; 
        aocio::line_tokenise(toks.at(1), ",", "", str_nums);
        for (const auto& str : str_nums) {
            sr.damaged_groups.push_back(aocio::parse_num(str)); 
        }

        result.push_back(sr); 
    }
}

bool is_valid_arrangement(const SpringRecord& s)
{
      assert(s.condition.find("?") == std::string::npos); 

      size_t str_idx = 0; 
      for (size_t i = 0; i < s.damaged_groups.size(); ++i) {
        size_t damaged_len = s.damaged_groups.at(i); 
        size_t start = s.condition.find('#', str_idx); 
        if (start == std::string::npos) {
            return false; 
        } else if (s.condition.size() - start < damaged_len) {
            return false;
        }

        size_t end = s.condition.find_first_not_of("#", start); 
        if (end == std::string::npos) {
            end = s.condition.size(); 
        }
        assert(end >= start);
        size_t len = end - start; 

        if (len != damaged_len) {
            return false; 
        }

        str_idx = end; 
        if (i != s.damaged_groups.size() - 1) {
            if (str_idx >= s.condition.size()) {
                return false; 
            }
            if (s.condition.at(str_idx) != '.') { // Enforce gap.
                return false; 
            }
        }
      }

      if (str_idx < s.condition.size()) {
        return s.condition.find("#", str_idx) == std::string::npos;
      }

      return true;
}

int64_t find_arrangements_bruteforce(SpringRecord &s, size_t str_idx = 0, size_t depth = 0)
{
    int64_t total = 0; 
    auto unknown_idx = s.condition.find("?", str_idx);

    if (unknown_idx == std::string::npos) {
        bool is_valid = is_valid_arrangement(s); 
        return is_valid ? 1 : 0; 
    }

    // Try '#'.
    s.condition.at(unknown_idx) = '#';
    total += find_arrangements_bruteforce(s, unknown_idx + 1, depth + 1); 
    // Try '.'
    s.condition.at(unknown_idx) = '.';
    total += find_arrangements_bruteforce(s, unknown_idx + 1, depth + 1); 
    // Undo
    s.condition.at(unknown_idx) = '?';
    
    return total;
}

int64_t find_arrangements_backtrack(SpringRecord &s, size_t str_idx = 0, size_t dmg_group_idx = 0)
{
    if (dmg_group_idx == s.damaged_groups.size()) {
        std::replace(s.condition.begin(), s.condition.end(), '?', '.');
        return is_valid_arrangement(s) ? 1 : 0;
    }

    const size_t damaged_len = s.damaged_groups.at(dmg_group_idx);
    const std::string damaged_str = std::string(damaged_len, '#');
    assert(damaged_len == damaged_str.size());

    int64_t total = 0; 
    const std::string saved = s.condition.substr(0, s.condition.size()); 

    auto after_loop = [&s, &str_idx](size_t area_end) -> void {
        for (size_t i = str_idx; i < area_end; ++i) {
            if (s.condition.at(i) == '?') {
                s.condition.at(i) = '.';
            }
        }
        str_idx = area_end; 
        if (str_idx < s.condition.size()) {
            if (s.condition.at(str_idx) == '#') {
                str_idx += 1;
            } else {
                s.condition.at(str_idx) = '.'; // Enforce gap.
            }
        }
    };

    while (str_idx < s.condition.size()) {
        size_t area_start = s.condition.find_first_of("?#", str_idx);  
        size_t area_end = s.condition.find_first_not_of("?#", area_start); 
        if (area_end == std::string::npos) {
            area_end = s.condition.size();
        }

        if (area_start == std::string::npos) {
            after_loop(area_end); 
            continue;
        }

        assert(area_end >= area_start);
        size_t area_size = area_end - area_start;
        if (area_size < damaged_len) {
            after_loop(area_end);
            continue;
        }

        const size_t str_size = s.condition.size();

        for (size_t offset = 0; offset <= area_size - damaged_len; ++offset) {
            size_t start = area_start + offset; 
            size_t end = start + damaged_len; // one beyond the area
            if (end < s.condition.size()) {
                if (s.condition.at(end) == '#') {
                    continue;
                } else if (s.condition.at(end) == '?') { // Enforce gaps.
                    s.condition.at(end) = '.';
                }
            }
            assert(end - start == damaged_len); 

            // Try move.
            for (size_t i = str_idx; i < start; ++i) {
                if (s.condition.at(i) == '?') {
                    s.condition.at(i) = '.';
                }
            }
            s.condition.replace(start, damaged_len, damaged_str); 
            assert(str_size == s.condition.size());
            total += find_arrangements_backtrack(s, end, dmg_group_idx + 1); 

            // Undo move. 
            s.condition.replace(0, saved.size(), saved);
            assert(str_size == s.condition.size());
        }
        after_loop(area_end);
    }
    // Undo all moves.
    s.condition.replace(0, saved.size(), saved);
    return total;
}

int64_t part_one(const std::vector<std::string>& lines)
{
    std::vector<SpringRecord> springs; 
    parse_spring_records(lines, springs);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    int64_t total_bruteforce = 0; 
    for (auto& s : springs) {
        int64_t n = find_arrangements_bruteforce(s); 
        total_bruteforce += n; 
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Brute forced in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms\n";

    begin = std::chrono::steady_clock::now();
    int64_t total_backtrack = 0; 
    for (auto& s : springs) {
        int64_t n = find_arrangements_backtrack(s); 
        total_backtrack += n; 
    }
    end = std::chrono::steady_clock::now();
    std::cout << "Back tracked in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms\n";

    if (total_backtrack != total_bruteforce) {
        throw "Error: diverging solutions";
    } 
    return total_backtrack; 
}

int64_t part_two(const std::vector<std::string>& lines)
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