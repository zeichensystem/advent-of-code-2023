#include <string>
#include <unordered_map>
#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/5

    Solutions: 
        - Part 1: 51752125
        - Part 2: 12634632
        
    Notes: 
        - Part 1: range calculations cause integer overflow; add 'integer' to -fsanitise
        - Part 2: silly bug that I only fixed by looking at my solution for part 1
*/

struct IDRange {
    int64_t start_id, size; 

    friend std::ostream& operator<<(std::ostream& os, const IDRange& range) {
        return os << "start-idx: " << range.start_id << " size: " << range.size << "\n";
    }

    int64_t end_id() const
    {
        assert(size);
        return start_id + size - 1; 
    }

    bool is_empty() const {
        assert(size >= 0);
        return size == 0; 
    }

    bool operator==(const IDRange&) const = default; // C++20
};

IDRange idrange_get_overlap(const IDRange &r1, const IDRange& r2) {
        const IDRange& lower = r1.start_id <= r2.start_id ? r1 : r2;
        const IDRange& higher = (lower == r1) ? r2 : r1; 

        if (lower.end_id() < higher.start_id) { // Ranges don't overlap. 
            return IDRange {.size=0, .start_id=0}; 
        } 
        if (higher.start_id > lower.end_id()) { // Ranges don't overlap. 
            return IDRange {.size=0, .start_id=0}; 
        }

        int64_t start_id = higher.start_id; 
        int64_t end_id = higher.end_id() > lower.end_id() ? lower.end_id() : higher.end_id(); 
        int64_t size = end_id - start_id + 1;
        assert(end_id >= 0 && start_id >= 0);
        assert(end_id >= start_id);
        assert(size > 0); 
        return IDRange {.start_id = start_id, .size = size}; 
}

struct CatMapping {
    std::string src_name; 
    std::string dst_name;  
    std::vector <IDRange> src_ranges, dst_ranges; 
};

int64_t part_one(const std::vector<std::string>& lines)
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

int64_t part_two(const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, CatMapping> categories; 
    std::vector<IDRange> seed_ranges;
    
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
            if (tok == "seeds:") { // Part 2: Parse ranges.
                assert((std::ssize(tokens) - (tok_n + 1)) % 2 == 0);
                for (int i = tok_n + 1; i < std::ssize(tokens) - 1; i+=2) {
                    int64_t src_id = parse_num(tokens.at(i)); 
                    int64_t size = parse_num(tokens.at(i+1));
                    seed_ranges.push_back({.start_id=src_id, .size=size});
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
        std::vector<IDRange> next_seed_ranges {}; 
        for (const IDRange& range : seed_ranges) {
            if (current.src_ranges.size() != current.dst_ranges.size()) {
                throw "src and dst size not matching";
            }
            bool found_in_range = false; 
            int64_t min_id = INT64_MAX; 
            int64_t max_id = INT64_MIN; 
            for (int range_idx = 0; range_idx < std::ssize(current.src_ranges); ++range_idx) {
                const IDRange& cur_range_src = current.src_ranges.at(range_idx);
                const IDRange& cur_range_dst = current.dst_ranges.at(range_idx);
                const IDRange overlap_src = idrange_get_overlap(range, cur_range_src); 
                min_id = std::min(min_id, cur_range_src.start_id);
                max_id = std::max(max_id, cur_range_src.end_id());
                if (!overlap_src.is_empty()) {
                    int64_t delta = overlap_src.start_id - cur_range_src.start_id; 
                    assert(delta >= 0); 
                    const IDRange overlap_dst = {
                        .start_id = cur_range_dst.start_id + delta,
                        .size = overlap_src.size > cur_range_dst.size ? cur_range_dst.size : overlap_src.size
                    };
                    if (!overlap_dst.is_empty()) {
                        next_seed_ranges.push_back(overlap_dst);
                        found_in_range = true;
                    } 
                } 
            }

            if (!found_in_range) {
                next_seed_ranges.push_back(range); // Not in range: Source maps to itself. 
            } else {
                if (range.start_id < min_id) {
                    next_seed_ranges.push_back(IDRange {.start_id = range.start_id, .size = min_id - range.start_id});
                }
                if (range.end_id() > max_id) {
                    next_seed_ranges.push_back(IDRange {.start_id = max_id + 1, .size = range.end_id() - (max_id+1) + 1});
                }
            }
        }
        
        seed_ranges = next_seed_ranges;
        if (current.dst_name == "location") {
            break;
        } 
        current = categories[current.dst_name]; // Bugfix: This assigment goes after the if statement above, otherwise we break too early and get a wrong solution (cf. Notes).
    }

    auto range_cmp = [](const IDRange &a, const IDRange &b) -> bool {return a.start_id < b.start_id;};
    auto min = std::min_element(seed_ranges.begin(), seed_ranges.end(), range_cmp);
    
    if (min != seed_ranges.end()) {
        return min->start_id; 
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