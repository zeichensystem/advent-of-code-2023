#include <string>
#include <unordered_map>
#include <numeric>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/8

    Solutions: 
        - Part 1: 18113
        - Part 2: 12315788159977        
    Notes: 
        - Part 2 seems to take too long for my brute-force solution.
          Approach: For each node, calculate how many steps it takes until the next terminal node. 
          Notice: The input-data has "cycles": Once we reach the first terminal node, the cycle repeats. 
          In theory, those cycles could be longer etc., which would make this solution incorrect; cf. assertions l. 126-127
*/

using StrPair = std::pair<std::string, std::string>; 
using AdjacencyMap = std::unordered_map<std::string, StrPair>; 

void parse_graph(const std::vector<std::string>& lines, AdjacencyMap &adj_map_res, std::string &instruction_res)
{
    for (auto &line : lines) {
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t=(),", "", toks); 
        if (toks.size() == 1) { 
            instruction_res = toks.at(0); 
        } else if (toks.size() == 0) {
            continue; 
        } else {
            assert(toks.size() == 3);
            assert(!adj_map_res.contains(toks.at(0))); 
            adj_map_res.insert({toks.at(0), StrPair{toks.at(1), toks.at(2)}}); 
        }
    }
}

int part_one(const std::vector<std::string>& lines)
{
    AdjacencyMap adj_map; 
    std::string direction_instrs; 
    parse_graph(lines, adj_map, direction_instrs); 

    std::string current_node = "AAA"; 
    int instr_cnt = 0; 
    while (current_node != "ZZZ") {
        char instr = direction_instrs.at(instr_cnt % direction_instrs.size()); 
        assert(instr == 'L' || instr == 'R'); 
        StrPair adj = adj_map.at(current_node); 
        if (instr == 'L') {
            current_node = adj.first; 
        } else if (instr == 'R') {
            current_node = adj.second; 
        }
        ++instr_cnt; 
    }
    return instr_cnt; 
}

void parse_graph_two(const std::vector<std::string>& lines, AdjacencyMap& adj_map_res, std::string& instr_res, std::vector<std::string>& start_nodes)
{
    for (auto &line : lines) {
        std::vector<std::string> toks; 
        aocio::line_tokenise(line, " \t=(),", "", toks); 
        if (toks.size() == 1) { 
            instr_res = toks.at(0); 
        } else if (toks.size() == 0) {
            continue; 
        } else {
            assert(toks.size() == 3);
            assert(!adj_map_res.contains(toks.at(0))); 
            if (toks.at(0).back() == 'A') {
                start_nodes.push_back(toks.at(0)); 
            } 
            adj_map_res.insert({toks.at(0), StrPair{toks.at(1), toks.at(2)}}); 
        }
    }
}

int64_t part_two(const std::vector<std::string>& lines)
{
    AdjacencyMap adj_map; 
    std::string direction_instrs; 
    std::vector<std::string> current_nodes; 
    parse_graph_two(lines, adj_map, direction_instrs, current_nodes); 

    auto is_terminal_node = [](const std::string& n) -> bool {
        return n.back() == 'Z'; 
    };

    // 1.) Calculate how many steps it takes from each node to the next terminal node (if possible).
    std::unordered_map<std::string, std::pair<int64_t, std::string>> steps_to_next_terminal; 
    for (const auto& [start_node, _] : adj_map) { 
        std::string cur_node = start_node; 
        int64_t instr_cnt = 0; 
        size_t cycle_counter = 0; 
        do {
            char instr = direction_instrs.at(instr_cnt % direction_instrs.size()); 
            assert(instr == 'L' || instr == 'R'); 
            StrPair adj = adj_map.at(cur_node); 
            cur_node = (instr == 'L') ? adj.first : adj.second; 
            ++instr_cnt; 

            if (adj.first == adj.second) { // Detect infinite loops. 
                ++cycle_counter; 
            } else {
                cycle_counter = 0; 
            }
            if (cycle_counter > direction_instrs.size()) {
                std::cout << "Detected infinite loop\n";
                break; 
            }
        } while (!is_terminal_node(cur_node)); 
        
        steps_to_next_terminal.insert({start_node, {instr_cnt, cur_node}}); 
    }

    // 2.) Get the "cycles" for each path.
    std::vector<int64_t> cycles; 
    for (const auto& start_node : current_nodes) {
        auto [steps, node] = steps_to_next_terminal.at(start_node);
        std::cout << start_node << "->" << node << " (cycle after " << steps << " steps)\n";
        cycles.push_back(steps); 
        assert(steps_to_next_terminal.at(node).first == steps);
        assert(steps_to_next_terminal.at(node).second == node);
    }

    // 3.) Find the smallest number divisible by all the cycle counts.
    int64_t steps = std::accumulate(cycles.begin(), cycles.end(), 1ll, [](int64_t x, int64_t y) -> int64_t {return std::lcm(x, y);}); 
    return steps;
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
        int64_t p2 = part_two(lines);
        std::cout << "Part 2: " << p2 << "\n";
    } catch (const char* err) {
        std::cerr << "Error: " << err << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}