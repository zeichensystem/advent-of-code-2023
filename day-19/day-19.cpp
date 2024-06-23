#include <unordered_map>
#include <array>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/19
  
    Solutions: 
        - Part 1: 348378
        - Part 2: 121158073425385
    Notes:  
        - Part 1: 
            - Essentially just the parsing, and then following each part from workflow to workflow :)
        - Part 2:
            - Was challenging, in a fun way! 
            - My approach is building a tree of all possible paths from the root "in" workflow to "A" or "R".
              (E.g. the "in" root node has one child node for each of its rules, plus one child if no rules match.)
            - For each node in the tree of paths, we can calculate a combined rule which a part has to match to 
              reach that node. (The "in" root node has a combined rule which matches all valid parts.)
            - After the tree of all possible paths was built, we can follow each path,
              and when "A" is the leaf node of the given path, we calculate the number of rating-combinations
              which satisfy the combined rule at the leaf node, and add it to the total.
            - Takes about 10 ms to solve both parts on my laptop. 
*/

enum class Operator {LessThan, GreaterThan}; 
enum class OperandType {x, m, a, s}; 

struct Rule 
{
    OperandType operand_type; 
    Operator cmp; 
    int operand_rhs; 
    std::string send_to; 

    Rule inverted() const 
    {
        Rule inv = *this;  
        if (cmp == Operator::LessThan) {
            inv.cmp = Operator::GreaterThan; 
            inv.operand_rhs = operand_rhs - 1;
        } else if (cmp == Operator::GreaterThan) {
            inv.cmp =  Operator::LessThan; 
            inv.operand_rhs = operand_rhs + 1;
        } else {
            assert(false); 
        }
        return inv;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rule& rule) 
    {
        char operand_sym = '?';
        if (rule.operand_type == OperandType::x) {
            operand_sym = 'x'; 
        } else if (rule.operand_type == OperandType::m) {
            operand_sym = 'm'; 
        } else if (rule.operand_type == OperandType::a) {
            operand_sym = 'a'; 
        } else if (rule.operand_type == OperandType::s) {
            operand_sym = 's'; 
        } else {
            assert(false);
        }
        char cmp_sym = '?'; 
        if (rule.cmp == Operator::LessThan) {
            cmp_sym = '<'; 
        } else if (rule.cmp == Operator::GreaterThan) {
            cmp_sym = '>'; 
        } else {
            assert(false);
        }
        return os << operand_sym << " " << cmp_sym << " " << rule.operand_rhs; 
    }
}; 

struct Part 
{
    // Merry
    int x, m, a, s;  

    int rating_sum() const {
        return x + m + a + s;
    }

    bool matches(const Rule& rule) const
    {
        int operand = 0; 
        switch (rule.operand_type)
        {
        case OperandType::x:
            operand = x;
            break;
        case OperandType::m: 
            operand = m;
            break; 
        case OperandType::a: 
            operand = a;
            break;
        case OperandType::s:
            operand = s; 
            break;
        default:
            throw std::invalid_argument("rule_execute: Invalid Operand");
        }

        if (rule.cmp == Operator::LessThan) {
            return operand < rule.operand_rhs; 
        } else if (rule.cmp == Operator::GreaterThan) {
            return operand > rule.operand_rhs;
        } else {
            throw std::invalid_argument("rule execute: Invalid Operator");
        }
    }
};

struct Workflow {
    std::vector<Rule> rules; 
    std::string no_rule_matches_send_to; 
};

void parse_workflow(const std::string& line, std::unordered_map<std::string, Workflow>& workflows)
{
    if (line.size() == 0) {
        return;
    }
    std::vector<std::string> toks; 
    aocio::line_tokenise(line, "{}<>,:", "{}<>,:", toks);

    if (toks.size() < 4) {
        throw std::invalid_argument("parse_workflow: Invalid workflow (too short)"); 
    }

    constexpr std::size_t rule_num_tokens = 6; 

    auto parse_rule = [&toks](std::size_t i) -> std::optional<Rule> 
    {
        if (i + rule_num_tokens >= toks.size() - 1) {
            return {};
        }
        Rule rule; 
        if (toks.at(i) == "x" || toks.at(i) == "m" || toks.at(i) == "a" || toks.at(i) == "s") {
            switch (toks.at(i).at(0))
            {
            case 'x':
                rule.operand_type = OperandType::x; 
                break;
            case 'm': 
                rule.operand_type = OperandType::m; 
                break; 
            case 'a': 
                rule.operand_type = OperandType::a; 
                break; 
            case 's': 
                rule.operand_type = OperandType::s;
                break;
            default:
                return {};
            }
        }

        if (toks.at(i + 1) == "<") {
            rule.cmp = Operator::LessThan; 
        } else if (toks.at(i + 1) == ">") {
            rule.cmp = Operator::GreaterThan; 
        } else {
            return {}; 
        }

        auto rhs_operand = aocio::parse_num(toks.at(i + 2)); 
        if (rhs_operand) {
            rule.operand_rhs = rhs_operand.value(); 
        } else {
            return {};
        }

        if (toks.at(i + 3) != ":") {
            return {}; 
        }

        rule.send_to = toks.at(i + 4);

        if (toks.at(i + 5) != ",") {
            return {}; 
        }

        return rule; 
    }; 

    if (toks.at(1) != "{" || toks.at(toks.size() - 1) != "}") {
        throw std::invalid_argument("parse_workflow: Missing { or }");
    }

    std::string workflow_name = toks.at(0); 
    Workflow workflow; 

    for (std::size_t i = 2; i < toks.size() - 1; i += rule_num_tokens) {
        if (i == toks.size() - 2) {
            workflow.no_rule_matches_send_to = toks.at(i); 
            if (workflows.contains(workflow_name)) {
                throw std::invalid_argument("parse_workflow: Duplicate workflow");
            } else {
                workflows.insert({workflow_name, workflow}); 
                return;
            }
        }
        auto rule = parse_rule(i);
        if (!rule) {
            std::string err_str = "parse_workflow: Invalid rule at pos " + std::to_string(i); 
            throw std::invalid_argument(err_str);
        } else {
            workflow.rules.push_back(rule.value());
        }

        assert(i + rule_num_tokens < toks.size());
    }

    assert(false);
}

void parse_parts(const std::string& line, std::vector<Part>& parts)
{
    if (line.size() == 0) {
        return;
    }
    std::vector<std::string> toks; 
    aocio::line_tokenise(line, ",{}", "{}", toks);

    if (toks.size() != 6) {
        std::cout << line << "\n";
        throw std::invalid_argument("parse_parts: Invalid length"); 
    }
    if (toks.at(0) != "{" || toks.back() != "}") {
        throw std::invalid_argument("parse_parts: Missing { or }"); 
    }

    Part part; 
    bool set_x = false, set_m = false, set_a = false, set_s = false; 
    for (std::size_t i = 1; i < toks.size() - 1; ++i) {
        std::vector<std::string> assignment_toks; 
        aocio::line_tokenise(toks.at(i), "=", "", assignment_toks); 
        if (assignment_toks.size() != 2) {
            throw std::invalid_argument("parse_parts: Invalid assignment"); 
        }

        char operand_sym = assignment_toks.at(0).at(0); 
        auto operand_val = aocio::parse_num(assignment_toks.at(1));
        if (!operand_val) {
            throw std::invalid_argument("parse_parts: Invalid assignment value"); 
        }

        switch (operand_sym)
        {
        case 'x':
            if (set_x) {
                throw std::invalid_argument("parse_parts: Duplicate assignment"); 
            }
            part.x = operand_val.value();
            set_x = true;
            break;

        case 'm': 
            if (set_m) {
                throw std::invalid_argument("parse_parts: Duplicate assignment"); 
            }
            part.m = operand_val.value();
            set_m = true;
            break;

        case 'a': 
            if (set_a) {
                throw std::invalid_argument("parse_parts: Duplicate assignment"); 
            }
            part.a = operand_val.value();
            set_a = true; 
            break; 

        case 's': 
            if (set_s) {
                throw std::invalid_argument("parse_parts: Duplicate assignment"); 
            }
            part.s = operand_val.value();
            set_s = true;
            break;   

        default:
            throw std::invalid_argument("parse_parts: Invalid assignment symbol"); 
            break;
        }
    }

    if (!(set_x && set_m && set_a && set_s)) {
        throw std::invalid_argument("parse_parts: Incomplete assignment"); 
    }

    parts.push_back(part);
}

int64_t part_one(const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, Workflow> workflows; 
    std::vector<Part> parts; 

    bool in_workflows = true; 
    for (std::string line : lines) {
        aocio::str_remove_whitespace(line);
        if (line.size() == 0) {
            in_workflows = false; 
            continue;
        }
        if (in_workflows) {
            parse_workflow(line, workflows); 
        } else { 
            parse_parts(line, parts);
        }
    }

    int64_t accepted_rating_sum = 0; 
    for (const Part& part : parts) {
        std::string workflow_name = "in"; 
        while (workflow_name != "A" && workflow_name != "R") {
            bool matched_rule = false; 
            for (const Rule& rule : workflows.at(workflow_name).rules) {
                if (part.matches(rule)) {
                    workflow_name = rule.send_to; 
                    matched_rule = true; 
                    break;
                }
            }
            if (!matched_rule) {
                workflow_name = workflows.at(workflow_name).no_rule_matches_send_to;
            }
        }
        if (workflow_name == "A") {
            accepted_rating_sum += part.rating_sum();
        }
    }
    return accepted_rating_sum;
}

struct CombinedRule 
{
    struct MinMax {
        int min = 1, max = 4000; 
    };

    std::array<MinMax, 4> min_max; // One MinMax for each of the OperandTypes x, m, a, s

    void combine(const Rule& rule) 
    {
        int min_max_idx = 0; 
        switch (rule.operand_type)
        {
        case OperandType::x:
            min_max_idx = 0; 
            break; 
        case OperandType::m: 
            min_max_idx = 1; 
            break; 
        case OperandType::a: 
            min_max_idx = 2; 
            break; 
        case OperandType::s: 
            min_max_idx = 3; 
            break; 
        default: 
            assert(false);
        }

        MinMax& min_max_elem = min_max.at(min_max_idx);
        if (rule.cmp == Operator::LessThan) {
            min_max_elem.max = std::min(min_max_elem.max, rule.operand_rhs - 1);
        } else if (rule.cmp == Operator::GreaterThan) {
            min_max_elem.min = std::max(min_max_elem.min, rule.operand_rhs + 1);
        } else {
            assert(false);
        }     
    }

    // How many distinct rating combinations can satisfy the combined rule:
    int64_t get_combinations() const  
    {
        int64_t total = 1; 
        for (const MinMax& mm : min_max) {
            if (mm.max < mm.min) {
                return 0; 
            }
            total *= (mm.max - mm.min) + 1;
        }
        return total;
    }
};

/*
    When a given workflow has n rules, we can take n + 1 distincts paths
    to reach the next workflows (one for each of the rules, and one if no rules match).
    We will build a tree from the "in" workflow until we reach "A" or "R". 
*/ 
struct Path {  
    std::string wf_name; 
    CombinedRule combined_rule; // The rules a part must satisfy to reach the current node of the path.
    std::vector<Path> children; 
};

/* 
    Builds a tree of all possible paths starting from parent.
*/
void build_paths(const std::unordered_map<std::string, Workflow>& workflows, Path& parent)
{
    const std::string& wf_name = parent.wf_name; 
    if (!workflows.contains(wf_name)) { // Reached a leaf-node ("A" or "R")
        return;
    }

    // 1.) Calculate all child nodes.
    const Workflow& wf = workflows.at(wf_name);
    std::vector<Rule> inv_rules;
    // 1.1.) Calculate the child nodes for each rule of the current node.
    for (const Rule& rule : wf.rules) {
        CombinedRule combined = parent.combined_rule; 
        combined.combine(rule);
        // For rule to match, all previous rules of the current node up to rule must not me matched.
        for (const Rule& inv_rule : inv_rules) { 
            combined.combine(inv_rule);
        }
        inv_rules.push_back(rule.inverted());
        parent.children.push_back(Path{.wf_name = rule.send_to, .combined_rule = combined}); 
    }
    // 1.2) Calculate the child node for the implicit rule if none of the rules in the current node are matched.
    CombinedRule combined = parent.combined_rule; 
    for (const Rule& inv_rule : inv_rules) {
        combined.combine(inv_rule);
    }
    parent.children.push_back(Path{.wf_name = wf.no_rule_matches_send_to, .combined_rule = combined}); 

    // 2.) For each child node, calculate their child nodes recursively. 
    for (Path& child : parent.children) {
        build_paths(workflows, child);
    }
}

/*
    Depth-first traversal of the tree. 
    When an "A"-leaf-node is reached, calculate the number of rating-combinations, and add it to the total.
*/
void follow_path(const std::unordered_map<std::string, Workflow>& workflows, const Path& parent, int64_t& accepted_ratings)
{
    if (!workflows.contains(parent.wf_name)) { // We Reached a leaf node (Either "R" or "A")
        if (parent.wf_name != "A") {
            return;
        }
        accepted_ratings += parent.combined_rule.get_combinations();
        return;
    }

    for (const Path& child : parent.children) {
        follow_path(workflows, child, accepted_ratings);
    }
}

int64_t part_two(const std::vector<std::string>& lines)
{
    std::unordered_map<std::string, Workflow> workflows; 

    for (std::string line : lines) {
        aocio::str_remove_whitespace(line);
        if (line.size() == 0) {
            break;
        } 
        parse_workflow(line, workflows); 
    }

    Path path = {.wf_name = "in"};
    build_paths(workflows, path);
    int64_t accepted_combinations = 0; 
    follow_path(workflows, path, accepted_combinations);

    return accepted_combinations;
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

    aocio::remove_leading_empty_lines(lines);
    if (!lines.size()) {
        std::cerr << "Error: " << "Input is empty";
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