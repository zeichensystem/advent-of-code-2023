#include <string>
#include <unordered_map>
#include <numeric>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/6

    Solutions: 
        - Part 1: 1108800
        - Part 2: 
        
    Notes: 
      
*/

struct Race {
    int64_t duration_ms, record_dist_mm; 
};

void parse_races(std::vector<Race> &result, const std::vector<std::string>& lines)
{
    assert(lines.size() == 2);

    std::vector<std::string> toks_time, toks_dist; 
    aocio::line_tokenise(lines.at(0), " \t", "", toks_time);
    assert(toks_time.at(0) == "Time:");
    aocio::line_tokenise(lines.at(1), " \t", "", toks_dist);
    assert(toks_dist.at(0) == "Distance:");

    assert(toks_time.size() == toks_dist.size() && toks_time.size() > 1); 


    auto parse_num = [](const std::string& tok) -> int64_t {
        size_t num_read = 0; 
        int64_t id = std::stoll(tok, &num_read);
        if (num_read == 0) {
            throw "Invalid token: expected number";
        }
        return id; 
    };

    for (size_t i = 1; i < toks_time.size(); ++i) {
        result.emplace_back(Race {.duration_ms = parse_num(toks_time.at(i)), .record_dist_mm = parse_num(toks_dist.at(i))});
    }
}

int64_t part_one(const std::vector<std::string>& lines)
{
    std::vector<Race> races; 
    parse_races(races, lines);

    int64_t win_combinations = 1;
    for (const Race &r : races) {
        int64_t win_cmbs = 0; 
        for (int hold_time = 0; hold_time <= r.duration_ms; ++hold_time) {
            int speed = hold_time; 
            int travel_time = r.duration_ms - hold_time; 
            int dist = travel_time * speed; 
            if (dist > r.record_dist_mm) {
                ++win_cmbs;
            }
        }
        win_combinations *= win_cmbs; 
    }

    return win_combinations;
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
        // int p2 = part_two(lines);
        // std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}