#include <unordered_map>
#include <set>
#include <array>
#include <queue>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/20
  
    Solutions: 
        - Part 1: 912199500
        - Part 2: 
    Notes:  

*/

enum class Pulse {Low, High}; 

class Network;

class Module 
{
protected: 
    std::unordered_map<std::string, Pulse> input_pulses; 
    std::vector<std::string> outputs; 
    Pulse state; 
    virtual void update_state() {};

public: 
    friend Network;
    const std::string name;

    Module(const std::string& name, const std::vector<std::string>& outputs) : outputs(outputs), name(name) 
    {
        state = Pulse::Low; 
    }; 

    virtual ~Module() = default;

    // Returns true if the module will send a pulse. 
    virtual bool receive_pulse(const std::string& sender_name, Pulse p) 
    {
        assert(input_pulses.contains(sender_name));
        input_pulses.at(sender_name) = p; 
        return true; 
    }

    virtual void forward_pulse(Network& network, std::set<std::string>& sends_pulse, bool print = false); // Defined after Network.
};

class Network 
{
public: 
    std::unordered_map<std::string, std::unique_ptr<Module>> modules; 

    void insert_module(std::unique_ptr<Module> mod) 
    {
        if (modules.contains(mod->name)) {
            throw std::invalid_argument("Network::insert_module: Duplicate module"); 
        }
        modules.insert({mod->name, std::move(mod)}); 
        update_connections();
    }

    int64_t push_button(int n = 1)
    {
        int low_pulses = 0; 
        int high_pulses = 0; 

        for (int i = 0; i < n; ++i) {
            low_pulses += 1; 
            std::queue<Module*> queue; 
            queue.push(modules.at("broadcaster").get());

            while (!queue.empty()) {
                Module *mod = queue.front();
                queue.pop();

                std::set<std::string> will_send_pulse; 
                mod->forward_pulse(*this, will_send_pulse); 
                if (mod->state == Pulse::Low) {
                    low_pulses += mod->outputs.size(); 
                } else {
                    high_pulses += mod->outputs.size();
                }
                for (const std::string& out_name : will_send_pulse) {
                    assert(modules.contains(out_name));
                    queue.push(modules.at(out_name).get());
                }
            }
        }
        return low_pulses * high_pulses;
    }

private: 
    void update_connections() 
    {
        for (auto& [input_name, input_mod] : modules) {
            for (const std::string& output_name : input_mod->outputs) {
                if (!modules.contains(output_name)) {
                    continue;
                }
                Module *output_mod = modules.at(output_name).get();
                output_mod->input_pulses.insert_or_assign(input_name, Pulse::Low);
            }
        }
    }
};
 
void Module::forward_pulse(Network& network, std::set<std::string>& will_send_pulse, bool print) 
{
        update_state();
        for (const std::string& output_name : outputs) {
            if (!network.modules.contains(output_name)) {
                continue;
            }
            Module *out_mod = network.modules.at(output_name).get();
            if (print) {
                std::string state_str = state == Pulse::Low ? " -low-> " : " -high-> ";
                std::cout << name << state_str << output_name << "\n";
            }
            bool will_send = out_mod->receive_pulse(name, state); 
            if (will_send) {
                will_send_pulse.insert(output_name);
            }
        }
}

class FlipFlop : public Module 
{
private: 
    bool on = false; 

public:
    FlipFlop(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs) 
    {
        state = Pulse::Low; 
    }

    bool receive_pulse(const std::string& sender_name, Pulse p) override
    {
        if (p == Pulse::High) {
            return false; 
        } else {
            state = on ? Pulse::Low : Pulse::High; 
            on = !on;
            return true;
        }
    }

};

class Conjunction : public Module 
{
public:
    Conjunction(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs) 
    {
        state = Pulse::Low; 
    }

protected:
    void update_state() override
    {
        auto input_low = std::find_if(input_pulses.cbegin(), input_pulses.cend(), [](const auto& name_pulse) {return name_pulse.second == Pulse::Low;});
        bool all_inputs_high = input_low == input_pulses.cend(); 
        if (all_inputs_high) {
            state = Pulse::Low; 
        } else {
            state = Pulse::High;
        }
    }
};

class Broadcast : public Module 
{
public: 
    Broadcast(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs)
    {
        state = Pulse::Low; 
    }

    bool receive_pulse(const std::string& sender_name, Pulse p) override
    {
        state = p; 
        return true; 
    }
};

void parse_network(const std::vector<std::string>& lines,  Network& network)
{
    for (std::string line : lines) {
        aocio::str_remove_whitespace(line);
        if (!line.size()) {
            continue;
        }
        std::size_t arrow_idx = line.find("->", 0); 
        if (arrow_idx == std::string::npos) {
            throw std::invalid_argument("parse_network: Missing arrow"); 
        }

        std::string lhs = line.substr(0, arrow_idx); 
        const std::string& rhs = line.substr(arrow_idx + 2, line.size()); 

        if (lhs.size() < 2 || rhs.size() < 1) {
            throw std::invalid_argument("parse_network: Assignment too short"); 
        }

        std::vector<std::string> dest_mods;
        aocio::line_tokenise(rhs, ",", "", dest_mods);

        if (dest_mods.size() == 0) {
            throw std::invalid_argument("parse_network: No destination modules"); 
        }

        const char module_sym = lhs.at(0); 

        if (module_sym != '%' && module_sym != '&') {
            if (lhs != "broadcaster") {
                throw std::invalid_argument("parse_network: Invalid module type"); 
            }
            std::unique_ptr<Module> mod = std::make_unique<Broadcast>(lhs, dest_mods);
            network.insert_module(std::move(mod));
        } else {
            lhs = lhs.substr(1, lhs.size()); 
            if (module_sym == '%') {
                std::unique_ptr<Module> mod = std::make_unique<FlipFlop>(lhs, dest_mods);
                network.insert_module(std::move(mod));
            } else {
                std::unique_ptr<Module> mod = std::make_unique<Conjunction>(lhs, dest_mods);
                network.insert_module(std::move(mod));
            }
        }
    }
}


int64_t part_one(const std::vector<std::string>& lines)
{
    Network network; 
    parse_network(lines, network);
    return network.push_button(1000);
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