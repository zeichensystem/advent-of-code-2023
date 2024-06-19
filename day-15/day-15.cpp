#include <array>
#include <numeric>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/15
  
    Solutions: 
        - Part 1: 512283
        - Part 2: 215827
    Notes:  
        - Part 2: Naive solution, but it still runs instantly. 
*/

bool is_ascii(char c)
{
    return c >= 0 && c <= 127; 
}

uint8_t hash(std::string_view s)
{
    uint8_t h = 0; 
    for (char c : s) {
        if (!is_ascii(c)) {
            throw "hash: not an ASCII-character";
        }
        // ((h + c) * 17) % 256 <=> ((h + c) % 256 * 17 % 256) % 256 <=> ((h + c) % 256 * 17) % 256
        h = (uint8_t)(h + (uint8_t)c); 
        h = (uint8_t)(h * 17);
    }
    return h; 
}

int part_one(const std::vector<std::string>& lines)
{
    int total_hash = 0; 
    std::string_view line = lines.at(0);
    size_t start_idx = 0; 
    
    while (start_idx < line.size()) {
        size_t end_idx = line.find(",", start_idx); 
        if (end_idx == std::string::npos) {
            end_idx = line.size();
        }
        assert(end_idx >= start_idx);
        std::string_view word = line.substr(start_idx, end_idx - start_idx); 
        total_hash += hash(word); 
        start_idx = end_idx + 1; 
    }

    return total_hash;
}

struct Lens {
    std::string lbl; 
    int focal_len; 
};

inline bool operator==(const Lens& lens, std::string_view lbl) {
    return lens.lbl == lbl; 
}

inline bool operator==(std::string_view lbl, const Lens& lens) {
    return lens == lbl; 
}

struct Box 
{
    std::vector<Lens> lenses;  
    uint8_t idx; 

    void insert_or_update_lens(const std::string& lbl, int focal_len)
    {
        if (auto lens = std::find(lenses.begin(), lenses.end(), lbl); lens != lenses.end()) { 
            lens->focal_len = focal_len;
        } else {
            lenses.push_back(Lens{lbl, focal_len});
        }
    }
    
    void remove_lens(const std::string& lbl)
    {
        if (auto lens = std::find(lenses.begin(), lenses.end(), lbl); lens != lenses.end()) {
            lenses.erase(lens); 
        }
    }

    int calc_focusing_power() const
    {
        int total = 0, slot_num = 1;
        for (const Lens& lens : lenses) {
            int focusing_pwr = (idx + 1) * slot_num++ * lens.focal_len; 
            total += focusing_pwr; 
        }
        return total;
    }
}; 

int part_two(const std::vector<std::string>& lines)
{
    std::array<Box, 256> boxes; 
    for (size_t i = 0; i < boxes.size(); ++i) {
        boxes.at(i).idx = (uint8_t)i; 
    }
    
    std::string_view line = lines.at(0);
    size_t start_idx = 0; 

    while (start_idx < line.size()) {
        size_t end_idx = line.find(",", start_idx); 
        if (end_idx == std::string::npos) {
            end_idx = line.size();
        }
        assert(end_idx >= start_idx);
        const std::string_view word = line.substr(start_idx, end_idx - start_idx); 

        size_t cmd_idx = word.find_first_of("-="); 
        if (cmd_idx == std::string::npos) {
            throw "Missing command";
        }
        const char cmd = word.at(cmd_idx); 

        const std::string lens_lbl = std::string{word.substr(0, cmd_idx)};
        Box &box = boxes.at(hash(lens_lbl));

        if (cmd == '-') {
            box.remove_lens(lens_lbl); 
        } else if (cmd == '=') {
            const std::string focal_len_str = std::string{word.substr(cmd_idx + 1, word.size())}; 
            int focal_len = aocio::parse_num(focal_len_str).value(); 
            box.insert_or_update_lens(lens_lbl, focal_len);
        } else {
            throw "Invalid command";
        }

        start_idx = end_idx + 1; 
    }

    return std::accumulate(boxes.begin(), boxes.end(), 0, [](int total, const Box& box) {return total + box.calc_focusing_power();});
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