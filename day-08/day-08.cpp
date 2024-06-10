#include <string>
#include <unordered_map>

#include "../aocio/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/7

    Solutions: 
        - Part 1: 18113
        - Part 2: 
        
    Notes: 
        
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

int part_two(const std::vector<std::string>& lines)
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