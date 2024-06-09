#include <string>
#include <numeric>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/6

    Solutions: 
        - Part 1: 1108800
        - Part 2: 36919753
        
    Notes: 
      - There's a closed form solution (see Part 2).
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
        int64_t n = std::stoll(tok, &num_read);
        if (num_read == 0) {
            throw "Invalid token: expected number";
        }
        return n; 
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

Race parse_races_part_two(const std::vector<std::string>& lines)
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
        int64_t n = std::stoll(tok, &num_read);
        if (num_read == 0) {
            throw "Invalid token: expected number";
        }
        return n; 
    };

    auto pow_10 = [](int64_t exp) -> int64_t {
        int64_t res = 1; 
        for (int i = 0; i < exp; ++i) {
            res *= 10; 
        }
        return res; 
    };

    auto num_digits = [](int64_t n) -> int64_t {
        int64_t digits = 1; 
        while (n >= 10) {
            n /= 10; 
            ++digits;
        }
        return digits;
    };

    Race r {.duration_ms = 0, .record_dist_mm = 0};
    for (size_t i = 1; i < toks_time.size(); ++i) {
        int64_t duration_digit = parse_num(toks_time.at(i));
        int64_t dist_digit = parse_num(toks_dist.at(i));
        r.duration_ms = r.duration_ms * pow_10(num_digits(duration_digit)) + duration_digit; 
        r.record_dist_mm = r.record_dist_mm * pow_10(num_digits(dist_digit)) + dist_digit; 
    }
    return r; 
}

constexpr bool part_two_use_brute_force = false; 

int64_t part_two(const std::vector<std::string>& lines)
{
    Race r = parse_races_part_two(lines); 
    int64_t win_combinations = 0;

    if constexpr (part_two_use_brute_force) {
        std::cout << "Using brute force...\n";
        for (int64_t hold_time = 0; hold_time <= r.duration_ms; ++hold_time) {
            int64_t speed = hold_time; 
            int64_t travel_time = r.duration_ms - hold_time; 
            int64_t dist = travel_time * speed; 
            if (dist > r.record_dist_mm) {
                ++win_combinations;
            }
        }
        return win_combinations;
    }
    /*
        The brute-force solution above works and is fast enough, but there's a closed form solution: 
        travel_time * speed = dist(hold_time) 
        <=> (duration - hold_time) * hold_time = dist(hold_time)
        <=> (duration - hold_time) * hold_time = record_dist [let dist(hold_time) be record_dist]
        <=> dur * ht - ht^2 = rec
        <=> -1 * ht^2 + dur * ht - rec = 0
        <=> ht = (-dur ± sqrt(-dur^2 - 4 * -1 * -rec)) / (2 * -1)
    */
    int64_t discr = r.duration_ms * r.duration_ms - 4 * -1 * -r.record_dist_mm; 
    if (discr < 0) { // Quadratic equation has no solution.
        return 0; 
    }
    double min_hold_time_one = (-r.duration_ms + std::sqrtl(discr)) / -2.; 
    double min_hold_time_two = (-r.duration_ms - std::sqrtl(discr)) / -2.; 
    assert(min_hold_time_one >= 0 || min_hold_time_two >= 0);

    int64_t min_hold_time = std::floorl(min_hold_time_one >= 0 ? min_hold_time_one : min_hold_time_two);
    while ((r.duration_ms - min_hold_time) * min_hold_time <= r.record_dist_mm) {
        min_hold_time += 1; 
    }
    win_combinations = r.duration_ms - min_hold_time * 2 + 1;
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
        int64_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}