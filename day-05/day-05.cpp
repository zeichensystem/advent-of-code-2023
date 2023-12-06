#include <string>
#include <unordered_map>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/5

    Solutions: 
        - Part 1: 51752125
        - Part 2: 
        
    Notes: 
*/

struct IDRange {
    int64_t start_id, size; 
    friend std::ostream& operator<<(std::ostream& os, const IDRange& range) {
        return os << "start-idx: " << range.start_id << " size: " << range.size << "\n";
    }
};

struct CatMapping {
    std::string src_name; 
    std::string dst_name;  
    std::vector <IDRange> src_ranges, dst_ranges; 
};

int part_one(const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, CatMapping> categories; 
    std::vector<int64_t> seeds;
    
    auto parse_num = [](const std::string& tok) -> int64_t {
        size_t num_read = 0; 
        int64_t id = std::stoll(tok, &num_read);
        if (num_read == 0) {
            throw "Invalid token: expected number";
        }
        return id; 
    };

    bool in_map = false; 
    std::string src_cat_name = "";
    std::string dest_cat_name = "";
    for (const std::string& line : lines) {
        if (!line.size()) {
            in_map = false; 
        }

        std::vector<std::string> tokens; 
        aocio::line_tokenise(line, " \t-", "", tokens);
        
        if (in_map)  {
            if (tokens.size() != 3) {
                throw "Amount of numbers within mapping section must be 3 (dst, src, range)";
            }
            int64_t dst_id = parse_num(tokens.at(0)); 
            int64_t src_id = parse_num(tokens.at(1));
            int64_t range_n = parse_num(tokens.at(2));
            categories[src_cat_name].src_ranges.push_back({.start_id=src_id, .size=range_n}); 
            categories[src_cat_name].dst_ranges.push_back({.start_id=dst_id, .size=range_n}); 
            continue;
        }

        for (int tok_n = 0; tok_n < std::ssize(tokens); ++tok_n) {
            std::string tok = tokens[tok_n];
            if (tok == "seeds:") {
                for (int i = tok_n + 1; i < std::ssize(tokens); ++i) {
                    seeds.push_back(parse_num(tokens[i]));
                }
                break; 
            } 

            if (tok == "to") { //src_cat-to-dst_cat
                if (tok_n <= 0 && tok_n + 1 >= std::ssize(tokens)) {
                    throw "Cannot parse mapping-section-title.";
                }
                src_cat_name = tokens[tok_n - 1]; 
                dest_cat_name = tokens[tok_n + 1];
                CatMapping src_cat = {.dst_name=dest_cat_name, .src_name=src_cat_name};
                categories.insert({src_cat_name, src_cat});
                in_map = true; 
                break; 
            } 
        }
    }

    CatMapping& current = categories["seed"]; 
    while (true) {
        std::vector<int64_t> next_seeds {}; 
        for (int64_t id : seeds) {
            if (current.src_ranges.size() != current.dst_ranges.size()) {
                throw "src and dst size not matching";
            }
            bool found_in_range = false; 
            for (int range_idx = 0; range_idx < std::ssize(current.src_ranges); ++range_idx) {
                const IDRange& cur_range_src = current.src_ranges.at(range_idx);
                const IDRange& cur_range_dst = current.dst_ranges.at(range_idx);
                if (id >= cur_range_src.start_id && id < cur_range_src.start_id + cur_range_src.size) {
                    next_seeds.push_back(cur_range_dst.start_id + (id - cur_range_src.start_id)); 
                    found_in_range = true; 
                    break;
                } 
            }
            if (!found_in_range) {
                next_seeds.push_back(id); // Not in range: Source maps to itself. 
            }
        }

        seeds = next_seeds;

        // std::cout << "From " << current.src_name << " to " << current.dst_name << "\n";
        // for (int64_t id : seeds) { // Print current ids. 
        //     std::cout << id << " ";
        // }
        // std::cout << "\n";

        if (current.dst_name == "location") {
            break;
        }
        current = categories[current.dst_name];
    }

    auto min = std::min_element(seeds.begin(), seeds.end());

    if (min != seeds.end()) {
        return *min;
    } else {
        throw "No location number (cannot find minimum)"; 
    }
}

int main()
{
    aocio::print_day();
    std::vector<std::string> lines;
    std::string_view fname = AOC_INPUT_DIR"input.txt";
    bool file_loaded = aocio::file_getlines(fname, lines);
    if (!file_loaded) {
        std::cerr << "Error: " << "File '" << fname << "' not found\n";
        return -1;
    }    

    try {
        int64_t p1 = part_one(lines);  
        std::cout << "Part 1: " << p1 << "\n";
        // int p2 = part_two(lines);
        // std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return -1;
    }
    return 0;
}