#include "../aoclib/aocio.hpp"
#include "../aoclib/vec.hpp"

/*
    Problem: https://adventofcode.com/2023/day/18
  
    Solutions: 
        - Part 1: 48652
        - Part 2: 45757884535661
    Notes:  
        - First idea: Calculate the area of the polygon 
        -> found https://en.wikipedia.org/wiki/Shoelace_formula#Trapezoid_formula_2 (last retrieved 2024-06-22)
        - This yielded the wrong answer. 
        -> found https://en.wikipedia.org/wiki/Pick's_theorem (last retrieved 2024-06-22)
           to calculate the inside points from the boundary points and area of the polygon

        - I think the derivation of the trapezoid formula is intuitive 
         (the German wikipedia has a nice diagram, cf. https://de.wikipedia.org/wiki/Gaußsche_Trapezformel#/media/Datei:Trapez-formel-prinz.svg), 
         but I don't understand how Pick's theorem works. 
*/

using Vec2 = typename aocutil::Vec2<int64_t>; 

std::pair<Vec2, Vec2> parse_edge(const std::string& line, Vec2& prev_end_vert, int64_t& boundary_points, bool part_2 = false)
{
    std::pair<Vec2, Vec2> edge; 

    std::vector<std::string> toks; 
    aocio::line_tokenise(line, " ", "", toks);
    assert(toks.size() == 3); 

    std::string dir = toks.at(0);
    int64_t steps = aocio::parse_num(toks.at(1)).value(); 

    if (part_2) {
        std::string clr_str = toks.at(2); 
        // Remove leading '(#' and trailing ')'
        clr_str =  clr_str.substr(2, clr_str.size()); 
        clr_str = clr_str.substr(0, clr_str.size() - 1); 
        // Now we have a six digit hex number; the first five digits represent the steps.
        steps = aocio::parse_hex(clr_str.substr(0, 5)).value();  
        // The last digit is a number representing the direction.
        int dir_int = aocio::parse_hex(clr_str.substr(5, 1)).value();
        assert(dir_int <= 3); 
        if (dir_int == 0) {
            dir = "R"; 
        } else if (dir_int == 1) {
            dir = "D"; 
        } else if (dir_int == 2) {
            dir = "L";
        } else {
            dir = "U";
        }
    }

    if (dir == "R") {
        edge.second = prev_end_vert + Vec2{steps, 0}; 
    } else if (dir == "L") {
        edge.second = prev_end_vert - Vec2{steps, 0}; 
    } 
    else if (dir == "U") {
        edge.second = prev_end_vert - Vec2{0, steps}; 
    } else if (dir == "D") {
        edge.second = prev_end_vert + Vec2{0, steps}; 
    } else {
        assert(false);
    }

    edge.first = prev_end_vert; 
    prev_end_vert = edge.second; 
    boundary_points += steps;
    return edge;
}

int64_t part_one(const std::vector<std::string>& lines, bool part_2 = false)
{
    Vec2 prev_end_vert = {0, 0}; 

    int64_t area = 0; 
    int64_t boundary_points = 0; 
    for (const auto& line : lines) {
        if (!line.size()) {
            continue;
        }
        std::string clr_str; 
        std::pair<Vec2, Vec2> edge = parse_edge(line, prev_end_vert, boundary_points, part_2); 
        area += (edge.first.y + edge.second.y) * (edge.first.x - edge.second.x); // Trapezoid formula. 
    }
    area = std::abs(area) / 2; // Trapezoid formula.  

    int64_t inside_points = area - boundary_points / 2 + 1; // Re-arranged Pick's theorem: A = i + b/2 - 1
    return inside_points + boundary_points;
}

int64_t part_two(const std::vector<std::string>& lines)
{
    return part_one(lines, true);
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
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << "\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}