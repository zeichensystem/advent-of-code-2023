#include <unordered_map>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/2

    Solutions:
        - Part 1: 2528
        - Part 2: 67363
*/

int part_n(const std::vector<std::string> &games, const std::unordered_map<std::string, int> &game_limits, int part_n)
{
    if (part_n != 1 && part_n != 2) {
        throw "Invalid part_n";
    }

    int solution_sum = 0; 
    for (const std::string& game : games) {
        std::istringstream is {game};
        std::string game_str, colon; int game_id = -1; 
        is >> game_str >> game_id >> colon;
        if (is.fail() || game_str != "Game" || colon != ":" || game_id == -1) {
            throw "Invalid Game ID";
        }

        std::unordered_map<std::string, int> game_cubes; 
        std::string colour {""}; int clr_amount {-1};
        while (!is.eof() && is >> clr_amount >> colour) {
            if (is.fail() || !colour.size() || clr_amount == -1) {
                throw "Invalid Game data";
            }
            if (colour.back() == ',' || colour.back() == ';') {
                colour.pop_back();
            }
            if (!game_cubes.contains(colour) || clr_amount > game_cubes[colour]) { // Note: Short-circuit eval.
                game_cubes[colour] = clr_amount;
            } 
        }

        if (part_n == 1) { 
            bool possible = true;
            for (const auto& [clr, amount] : game_cubes) {
                if (game_limits.contains(clr) && amount > game_limits.at(clr)) {
                    possible = false;
                    break;
                }
            }
            if (possible) {
                solution_sum += game_id;
            }
        } else if (part_n == 2) {
            int pwr = 1;
            for (const auto& [clr, amount] : game_cubes) {
                pwr *= amount;
            }
            solution_sum += pwr;
        }
    }
    return solution_sum;
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

    // 12 red cubes, 13 green cubes, and 14 blue cubes.
    std::unordered_map<std::string, int> game_limits {
        {"red", 12}, 
        {"green", 13},
        {"blue", 14}
    };

    try {
        int p1 = part_n(lines, game_limits, 1);  
        int p2 = part_n(lines, game_limits, 2);
        std::cout << "Part 1: " << p1 << "\n";
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return -1;
    }

    return 0;
}