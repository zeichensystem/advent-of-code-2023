#include <string>
#include <numeric>
#include <array>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/7

    Solutions: 
        - Part 1: 251806792
        - Part 2: 
        
    Notes: 
*/

constexpr std::array<char, 13> card_labels = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
enum class HandType {NumTypes = 7, FiveOfKind=6, FourOfKind=5, FullHouse=4, ThreeOfKind=3, TwoPair=2, OnePair=1, HighCard=0};
const std::array<std::string, static_cast<size_t>(HandType::NumTypes)> hand_type_to_str {"High Card", "One Pair", "Two Pair", "Three of kind", "Full House", "Four of kind", "Five of kind"};

int label_value(char lbl)
{
    const auto found_idx = std::find(card_labels.begin(), card_labels.end(), lbl); 
    assert(found_idx != std::end(card_labels));
    int value = std::distance(card_labels.begin(), found_idx);
    assert(value >= 0 && value < std::ssize(card_labels));
    return value; 
}

class Hand 
{
public: 
    std::array<char, 5> cards; 
    int bid; 

    Hand(const std::string &card_str, int bid) : bid{bid} 
    {
        assert(card_str.size() == 5);
        int i = 0; 
        for (const char &card : card_str) {
            cards.at(i++) = card; 
        }
    }

    friend std::ostream& operator<<(std::ostream &os, const Hand &h) {
        for (char card : h.cards) {
            os << card << " ";
        }

        return os << "(" << hand_type_to_str.at( static_cast<int>(h.type()) ) << ", bid: " << h.bid << ")\n";
    }

    auto operator<=>(const Hand &other) const
    {
        assert(cards.size() == other.cards.size());

        if (type() < other.type()) {
            return std::strong_ordering::less; 
        } else if (type() > other.type()) {
            return std::strong_ordering::greater; 
        }
        assert(type() == other.type());

        for (size_t i = 0; i < cards.size(); ++i) {
            if (label_value(cards.at(i)) < label_value(other.cards.at(i))) {
                return std::strong_ordering::less; 
            } else if (label_value(cards.at(i)) > label_value(other.cards.at(i))) {
                return std::strong_ordering::greater; 
            }
        }
        return std::strong_ordering::equal; 
    }

    bool operator==(const Hand &other) const
    {
        assert(cards.size() == other.cards.size());
        bool same_type = type() == other.type(); 
        if (!same_type) {
            return false; 
        }
        for (size_t i = 0; i < cards.size(); ++i) {
            if (cards.at(i) != other.cards.at(i)) {
                return false; 
            }
        }
        return true; 
    }

    HandType type() const 
    {
        std::array<int, card_labels.size()> kind_count {0};
        for (char c : cards) {
            kind_count.at(label_value(c)) += 1; 
        }

        bool has_three_of_kind = false;
        int pair_cnt = 0; 
        for (int cnt : kind_count) {
            if (cnt == 5) {
                return HandType::FiveOfKind; 
            } else if (cnt == 4) {
                return HandType::FourOfKind; 
            } 
            if (cnt == 3) {
                has_three_of_kind = true; 
            } else if (cnt == 2) {
                ++pair_cnt;
            }
        }

        if (has_three_of_kind && pair_cnt == 1) {
            return HandType::FullHouse;
        } else if (has_three_of_kind) {
            assert(pair_cnt == 0);
            return HandType::ThreeOfKind;
        } else if (pair_cnt == 2) {
            return HandType::TwoPair; 
        } else if (pair_cnt == 1) {
            return HandType::OnePair; 
        }  else {
            assert(!has_three_of_kind && !pair_cnt);
            assert(*std::max_element(kind_count.begin(), kind_count.end()) == 1);
            return HandType::HighCard;
        }
    }
}; 

void parse_hands(std::vector<Hand> &result, const std::vector<std::string>& lines)
{

    auto parse_num = [](const std::string &tok) -> int {
        size_t num_read = 0; 
        int n = std::stoi(tok, &num_read); 
        if (num_read == 0) {
            throw "Invalid token: expected number";
        }
        return n; 
    }; 
    for (auto &line : lines) {
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t", "", toks); 
        assert(toks.size() == 2); 
        int bid = parse_num(toks.at(1)); 
        result.emplace_back(Hand(toks.at(0), bid));
    }
}


int part_one(const std::vector<std::string>& lines)
{
    std::vector<Hand> hands; 
    parse_hands(hands, lines); 
    std::sort(hands.begin(), hands.end()); 

    int total_winnings = 0; 
    for (size_t i = 0; i < hands.size(); ++i) {
        int rank = i + 1; 
        total_winnings += hands.at(i).bid * rank;
    }

    return total_winnings;
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