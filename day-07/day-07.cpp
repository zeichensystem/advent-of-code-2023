#include <string>
#include <numeric>
#include <array>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/7

    Solutions: 
        - Part 1: 251806792
        - Part 2: 252113488
        
    Notes: 
        - I missed the bold "J cards are now the weakest" in the problem description of part 2 and wasted lots of time...
*/

constexpr std::array<char, 13> card_labels = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
enum class HandType {NumTypes = 7, FiveOfKind=6, FourOfKind=5, FullHouse=4, ThreeOfKind=3, TwoPair=2, OnePair=1, HighCard=0};
const std::array<std::string, static_cast<size_t>(HandType::NumTypes)> hand_type_to_str {"High Card", "One Pair", "Two Pair", "Three of a kind", "Full House", "Four of a kind", "Five of a kind"};

// Maps from the card label to the index in the card_labels-array (e.g. '2' -> 0, '3', -> 1 etc.)
size_t label_idx(char lbl)
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
    bool use_jokers; 

    Hand(const std::string &card_str, int bid, bool use_jokers = false) : bid{bid}, use_jokers{use_jokers} 
    {
        assert(card_str.size() == 5);
        int i = 0; 
        for (const char &card : card_str) {
            cards.at(i++) = card; 
        }
    }

    int label_value(char lbl) const
    {
        if (use_jokers && lbl == 'J') {
            return -1;
        }
        const auto found_idx = std::find(card_labels.begin(), card_labels.end(), lbl); 
        assert(found_idx != std::end(card_labels));
        int value = std::distance(card_labels.begin(), found_idx);
        assert(value >= 0 && value < std::ssize(card_labels));
        return value; 
    }

    friend std::ostream& operator<<(std::ostream &os, const Hand &h) {
        std::array<int, card_labels.size()> kind_count {0};
        for (char c : h.cards) {
            kind_count.at(label_idx(c)) += 1; 
        }
        int num_jokers = kind_count.at(label_idx('J')); 

        for (char card : h.cards) {
            os << card << " ";
        }

        return os << "(" << hand_type_to_str.at( static_cast<int>(h.type()) ) << ", bid: " << h.bid << ", jokers: " << num_jokers << ")\n";
    }

    auto operator<=>(const Hand &other) const
    {
        assert(cards.size() == other.cards.size());
        assert(use_jokers == other.use_jokers);

        if (type() < other.type()) {
            return std::strong_ordering::less; 
        } 
        if (type() > other.type()) {
            return std::strong_ordering::greater; 
        }
        assert(type() == other.type());

        for (size_t i = 0; i < cards.size(); ++i) {
            if (label_value(cards.at(i)) < other.label_value(other.cards.at(i))) {
                return std::strong_ordering::less; 
            } 
            if (label_value(cards.at(i)) > other.label_value(other.cards.at(i))) {
                return std::strong_ordering::greater; 
            }
        }

        assert(bid == other.bid);
        return std::strong_ordering::equal; 
    }

    HandType type() const 
    {
        std::array<int, card_labels.size()> kind_count {0};
        for (char c : cards) {
            kind_count.at(label_idx(c)) += 1; 
        }

        bool has_three_of_kind = false;
        int pair_cnt = 0; 
        int num_jokers = kind_count.at(label_idx('J')); 
        for (size_t i = 0; i < kind_count.size(); ++i) {
            if (use_jokers && i == label_idx('J')) {
                continue;
            }
            if (kind_count.at(i) == 5) {
                return HandType::FiveOfKind; 
            } else if (kind_count.at(i) == 4) {
                return (use_jokers && num_jokers > 0) ? HandType::FiveOfKind : HandType::FourOfKind; 
            } 
            if (kind_count.at(i) == 3) {
                has_three_of_kind = true; 
            } else if (kind_count.at(i) == 2) {
                ++pair_cnt;
            }
        }

        if (has_three_of_kind && pair_cnt == 1) {
            if (!use_jokers || num_jokers == 0) {
                return HandType::FullHouse; 
            } 
            assert(num_jokers != 1);
            assert(num_jokers == 2 || num_jokers == 3);
            return HandType::FiveOfKind;
        } 
        else if (has_three_of_kind) {
            assert(pair_cnt == 0);
            if (!use_jokers || num_jokers == 0) {
                return HandType::ThreeOfKind;
            } else if (num_jokers == 1) {
                return HandType::FourOfKind; 
            } else {
                assert(num_jokers == 2);
                return HandType::FiveOfKind;
            } 
        } 
        else if (pair_cnt == 2) {
            if (!use_jokers || num_jokers == 0) {
                return HandType::TwoPair;
            } else {
                assert(num_jokers == 1);
                return HandType::FullHouse; 
            }
        } 
        else if (pair_cnt == 1) {
            if (!use_jokers || num_jokers == 0) {
                return HandType::OnePair;
            } else if (num_jokers == 1) {
                return HandType::ThreeOfKind; 
            } else if (num_jokers == 2) {
                return HandType::FourOfKind; 
            } else {
                return HandType::FiveOfKind;
            }
        }  
        else {
            assert(!has_three_of_kind && !pair_cnt);
            assert(*std::max_element(kind_count.begin(), kind_count.end()) == (use_jokers ? std::max(num_jokers, 1) : 1));
            if (!use_jokers || num_jokers == 0) {
                return HandType::HighCard;
            } else if (num_jokers == 1) {
                return HandType::OnePair; 
            } else if (num_jokers == 2) {
                return HandType::ThreeOfKind; 
            } else if (num_jokers == 3) {
                return HandType::FourOfKind;
            } else {
                return HandType::FiveOfKind;
            }
        }
    }
}; 

void parse_hands(std::vector<Hand> &result, const std::vector<std::string>& lines, bool use_jokers = false)
{

    for (auto &line : lines) {
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t", "", toks); 
        assert(toks.size() == 2); 
        int bid = aocio::parse_num(toks.at(1)).value(); 
        result.emplace_back(Hand(toks.at(0), bid, use_jokers));
    }
}

int part_one(const std::vector<std::string>& lines, bool use_jokers = false)
{
    std::vector<Hand> hands; 
    parse_hands(hands, lines, use_jokers); 
    std::sort(hands.begin(), hands.end()); 

    int total_winnings = 0; 
    for (size_t i = 0; i < hands.size(); ++i) {
        int rank = i + 1; 
        total_winnings += hands.at(i).bid * rank;
    }

    return total_winnings;
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