#include <string>
#include <unordered_set>
#include <unordered_map>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/4

    Solutions:
        - Part 1: 32609
        - Part 2: 14624680

*/

int part_one(const std::vector<std::string>& lines)
{
    int cards_total_points = 0; 

    for (const std::string& line : lines ) {
        std::vector<std::string> card_tokens; 
        aocio::line_tokenise(line, " \t|:", "|:", card_tokens);
        if (card_tokens.size() <= 3 || card_tokens.at(2) != ":") {
            throw "Invalid card line";
        }

        int card_points = 0; 
        std::unordered_set<int> winning_numbers; 
        bool section_winning_numbers = true; 

        for (auto tok_i = card_tokens.begin() + 3; tok_i != card_tokens.end(); ++tok_i) {
            if (*tok_i == "|") {
                section_winning_numbers = false; 
                continue;
            } 
            size_t num_read = 0; 
            int num = std::stoi(*tok_i, &num_read, 10);
            if (!num_read) {
                throw "Invalid number token";
            }
            if (section_winning_numbers) {
                winning_numbers.insert(num);
            } else if (winning_numbers.contains(num)) {
                card_points = !card_points ? 1 : card_points * 2; 
            }
        }
        cards_total_points += card_points;
    }

    return cards_total_points;
}

int part_two(const std::vector<std::string>& lines)
{
    int cards_total = 0; 

    std::unordered_map<int, int> card_count; 

    auto update_card_count = [&card_count](int card_id, int amount) {
        if (card_count.contains(card_id)) {
            card_count[card_id] += amount;
        } else {
            card_count.insert({card_id, amount});
        }
    };

    for (const std::string& line : lines ) {
        std::vector<std::string> card_tokens; 
        aocio::line_tokenise(line, " \t|:", "|:", card_tokens);
        if (card_tokens.size() <= 3 || card_tokens.at(0) != "Card" || card_tokens.at(2) != ":") {
            throw "Invalid card line";
        }

        size_t num_read_card_id = 0; 
        const int card_id = std::stoi(card_tokens.at(1), &num_read_card_id);
        if (!num_read_card_id) {
            throw "Invalid Card id";
        }
        update_card_count(card_id, 1);

        int matching = 0; 
        std::unordered_set<int> winning_numbers; 
        bool section_winning_numbers = true; 
        for (auto tok_i = card_tokens.begin() + 3; tok_i != card_tokens.end(); ++tok_i) {
            if (*tok_i == "|") {
                section_winning_numbers = false; 
                continue;
            } 
            size_t num_read = 0; 
            int num = std::stoi(*tok_i, &num_read);
            if (!num_read) {
                throw "Invalid number token";
            }
            if (section_winning_numbers) {
                winning_numbers.insert(num);
            } else if (winning_numbers.contains(num)) {
                ++matching; // Don't multiply by two like in part 1. 
            }
        }

        for (int i = 1; i <= matching; ++i) {
            update_card_count(card_id + i, card_count[card_id]);
        }
    }

    for (const auto& [id, cnt] : card_count) {
        cards_total += cnt;
    }

    return cards_total;
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
        int p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return -1;
    }

    return 0;
}