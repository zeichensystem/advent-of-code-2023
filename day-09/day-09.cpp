#include <string>
#include <unordered_map>
#include <numeric>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/9

    Solutions: 
        - Part 1: 1708206096
        - Part 2: 1050     
    Notes: 
        - That one was easy I think! 
        - Learned about std::adjacent_difference and std::back_inserter
*/

void parse_histories(const std::vector<std::string>& lines, std::vector<std::vector<int>> &result)
{
    for (auto &line : lines) {
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t", "", toks); 
        std::vector<int> history; 
        for (const auto &tok : toks) {
            history.push_back(aocio::parse_num(tok).value());
       }
       result.push_back(history); 
    }
}

int part_one(const std::vector<std::string>& lines, bool backwards = false)
{
    std::vector<std::vector<int>> histories; 
    parse_histories(lines, histories); 

    int result = 0; 
    for (const std::vector<int>& history : histories) {
        std::vector<std::vector<int>> diffs; 
        diffs.push_back(history); 
        do {
            const std::vector<int>& prev = diffs.back(); 
            assert(prev.size() > 1); 
            std::vector<int> diff; 
            std::adjacent_difference(prev.begin(), prev.end(), std::back_inserter(diff)); 
            diff.erase(diff.begin());
            assert(diff.size() == prev.size() - 1); 
            diffs.push_back(diff); 
        } while (!std::all_of(diffs.back().begin(), diffs.back().end(), [](int a) -> bool {return a == 0;}));

        for (auto d = diffs.rbegin(), prev = diffs.rend(); d != diffs.rend(); prev = d++) {
            if (d == diffs.rbegin()) {
                d->push_back(0); 
            } else {
                // Notice: We still push next to the end of the vector if backwards == true.
                int next = !backwards ? d->back() + prev->back() : d->front() - prev->back(); 
                d->push_back(next); 
            } 
        } 
        result += diffs.front().back(); 
    }
    return result; 
}

int part_two(const std::vector<std::string>& lines)
{
    return part_one(lines, true);
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