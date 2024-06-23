#include <unordered_map>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/19
  
    Solutions: 
        - Part 1: 348378
        - Part 2: 
    Notes:  
*/

enum class Operator {LessThan, GreaterThan}; 
enum class OperandType {x, m, a, s}; 

struct Rule {
    OperandType operand_type; 
    Operator cmp; 
    int operand_rhs; 
    std::string send_to; 
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
    for (const auto& line : lines) {
        if (line.size() == 0) {
            in_workflows = false; 
        }

        std::string line_without_whitespace = ""; 
        auto is_ws = [](char c) -> bool { return c == ' ' || c == '\t'; }; 
        std::remove_copy_if(line.cbegin(), line.cend(), std::back_inserter(line_without_whitespace), is_ws);
        
        if (in_workflows) {
            parse_workflow(line_without_whitespace, workflows); 
        } else { 
            parse_parts(line_without_whitespace, parts);
        }
    }

    int64_t accepted_rating_sum = 0; 
    for (const Part& part : parts) {
        std::string workflow_name = "in"; 
        while (workflow_name != "A" && workflow_name != "R") {
            // std::cout << workflow_name << "->";
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
        // std::cout << workflow_name << "\n";

        if (workflow_name == "A") {
            accepted_rating_sum += part.rating_sum();
        }
    }

    return accepted_rating_sum;
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
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << "\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}