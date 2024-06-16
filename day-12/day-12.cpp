#include <unordered_map>
#include "../aocio/aocio.hpp"
#include "../lru-cache/lru-cache.hpp"

/*
    Problem: https://adventofcode.com/2023/day/12
  
    Solutions: 
        - Part 1: 8075
        - Part 2: 4232520187524
    Notes: 
        - Re-implemented find_arrangements to make it cacheable. 
        - Implemented a simple LRU-cache. It's not really useful, using an unordred_map is just as fast here. 
          But it should use less memory for smaller lru sizes, which is cool. 
*/

struct SpringRecord {
    std::string condition; 
    std::vector<int> damaged_groups; 

    void unfold()
    {
        std::string cond_unfolded = ""; 
        for (int i = 0; i < 5; ++i) {
            cond_unfolded += condition; 
            if (i != 4) {
                cond_unfolded += "?";
            }

        }
        condition = cond_unfolded; 

        std::vector<int> dg_unfolded; 
        for (int i = 0; i < 5; ++i) {
            for (int n : damaged_groups) {
                dg_unfolded.push_back(n);
            }
        }
        damaged_groups = dg_unfolded; 
    }

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

struct State {
    char cur_sym; 
    int str_idx, dmg_group_idx, dmg_spring_len; 
    bool operator==(const State&) const = default;
}; 

template<>
struct std::hash<State>
{
    std::size_t operator()(const State& s) const noexcept
    {
        std::size_t hash = std::hash<std::string>{}(std::string(1, s.cur_sym) + "," + std::to_string(s.str_idx) + "," + std::to_string(s.dmg_group_idx) + "," + std::to_string(s.dmg_spring_len));
        return hash;
    }
};

template <uint32_t LRU_SIZE>
int64_t find_arrangements(const SpringRecord &s, LRUCache<State, int64_t, LRU_SIZE>& lru, int dmg_group_idx = 0, int str_idx = 0, int dmg_spring_len = 0, char cur_sym = ' ')
{    
    if (cur_sym == ' ') {
        cur_sym = s.condition.at(0);
    }

    const State state = {.cur_sym = cur_sym, .str_idx = str_idx, .dmg_group_idx = dmg_group_idx, .dmg_spring_len = dmg_spring_len};
    
    if (lru.contains(state)) {
        return lru.get_copy(state);   
    }

    if (str_idx == std::ssize(s.condition)) {
        if (dmg_group_idx == std::ssize(s.damaged_groups) - 1 && dmg_spring_len == s.damaged_groups.at(dmg_group_idx)) {
            return 1; 
        } else {
            return 0; 
        }
    }

    char spring_sym = cur_sym; 
    switch (spring_sym) 
    {   
    case '#': {
        int64_t total = 0; 
        if (dmg_spring_len > s.damaged_groups.at(dmg_group_idx)) {
            total = 0; 
        } else {
            cur_sym = str_idx + 1 < std::ssize(s.condition) ? s.condition.at(str_idx + 1) : ' '; 
            total = find_arrangements(s, lru, dmg_group_idx, str_idx + 1, dmg_spring_len + 1, cur_sym); 
        }
        lru.insert(state, total);
        return total; 
    }

    case '?': {
        int64_t hash_subtotal = find_arrangements(s, lru, dmg_group_idx, str_idx, dmg_spring_len, '#');
        int64_t dot_subtotal = find_arrangements(s, lru, dmg_group_idx, str_idx, dmg_spring_len, '.'); 
        int64_t total = dot_subtotal + hash_subtotal; 
        lru.insert(state, total);
        return total;
    }

    case '.': {
        bool prev_was_damaged = dmg_spring_len != 0; 
        int64_t total = 0; 
        if (prev_was_damaged && dmg_spring_len != s.damaged_groups.at(dmg_group_idx)) {
            total = 0; 
        } else if (prev_was_damaged && dmg_group_idx + 1 == std::ssize(s.damaged_groups)) {
            total = dmg_spring_len == s.damaged_groups.at(dmg_group_idx) && s.condition.find("#", str_idx) == std::string::npos ? 1 : 0; 
        } else {
            cur_sym = str_idx + 1 < std::ssize(s.condition) ? s.condition.at(str_idx + 1) : ' '; 
            total = find_arrangements(s, lru, prev_was_damaged ? dmg_group_idx + 1 : dmg_group_idx, str_idx + 1, 0, cur_sym);
        }
        lru.insert(state, total);
        return total;
    }
    
    default:
        throw "Invalid spring condition.";
    }
}

constexpr uint32_t lru_size = 1024; 
using LRUCache_FindArr = LRUCache<State, int64_t, lru_size>; 
std::unique_ptr<LRUCache_FindArr> lru = std::make_unique<LRUCache_FindArr>(); 

int64_t part_one(const std::vector<std::string>& lines)
{
    std::vector<SpringRecord> springs; 
    parse_spring_records(lines, springs);

    assert(lru.get());

    int64_t total = 0; 
    for (auto& s : springs) {
        lru->clear(); 
        int64_t n = find_arrangements<lru_size>(s, *lru.get()); 
        total += n; 
    }
    
    return total; 
}

int64_t part_two(const std::vector<std::string>& lines)
{
    std::vector<SpringRecord> springs; 
    parse_spring_records(lines, springs);
    
    assert(lru.get()); 

    int64_t total = 0; 
    for (auto& s :springs) {
        s.unfold();
        lru->clear(); 
        total += find_arrangements<lru_size>(s, *lru.get()); 
    }

    return total; 
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