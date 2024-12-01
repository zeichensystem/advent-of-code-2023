#include <unordered_map>
#include <set>
#include <numeric>
#include <queue>
#include "../aoclib/aocio.hpp"

/*
    Problem: https://adventofcode.com/2023/day/20
  
    Solutions: 
        - Part 1: 912199500
        - Part 2: 237878264003759

    Notes:  
        - Part 1: - Decided to implement the different module types (flip-flop, conjunction, broadcast) as derived
                    classes from an abstract class "Module" -> polymorphism/dynamic dispatch.
                  - Also used owning smart pointers and non-owning regular pointers to access the polymorphic objects; 
                    cf. Bjarne Stroustrup, "A Tour of C++, 2nd Edition", Chapter 4.6, p. 63:
                        "[15] Access polymorphic objects through pointers and references (4.3)". 
                  
        - Part 2: - Only works for very specific inputs, reminds me of day 8.
                    Luckily enough, the input data is small enough to see that "rx" has just one input module which
                    happens to be a conjunction, and that module only has 4 input modules. We now know those 4 need 
                    to be high at the same time so rx gets a low pulse.

                  - The input data must have some properties for the solution to work: 
                    - There must be just one input module of "rx", which must be a conjunction
                    - Each of the input modules of that module (4 in my input) must all send "high" pulses predictably
                      after a certain amount of button presses in cycles (and those cycles must be small enough to find
                      by brute force; for my input they cycle after just a few thousand button presses).
                      This also means those "sub-networks" must not interfere with each other I think, but I'm not sure. 
*/

enum class Pulse {Low, High}; 

class Module;

class Network 
{
public: 
    std::unordered_map<std::string, std::unique_ptr<Module>> modules; // A network owns its modules. 
    Network(const std::vector<std::string>& lines);
    void insert_module(std::unique_ptr<Module> mod);
    int64_t push_button(int n = 1);

private: 
    void update_connections();
};


class Module 
{
protected: 
    Pulse state; 
    virtual void update_state() = 0;
     // Returns true if the module will send a pulse to its outputs after it received pulse p.
    virtual bool receive_pulse(const std::string& sender_name, Pulse p) = 0;

public: 
    const std::string name;
    std::unordered_map<std::string, Pulse> input_pulses; 
    std::vector<std::string> outputs; 

    Module(const std::string& name, const std::vector<std::string>& outputs) : state(Pulse::Low), name(name), outputs(outputs) {}; 
    
    virtual ~Module() = default;
    
    virtual Pulse forward_pulse(Network& network, std::set<std::string>& will_send_pulse, bool print = false) 
    {
        update_state();
        for (const std::string& output_name : outputs) {
            if (!network.modules.contains(output_name)) {
                continue;
            }
            Module *out_mod = network.modules.at(output_name).get();
            assert(out_mod);
            if (print) {
                std::string state_str = state == Pulse::Low ? " -low-> " : " -high-> ";
                std::cout << name << state_str << output_name << "\n";
            }
            bool will_send = out_mod->receive_pulse(name, state); 
            if (will_send) {
                will_send_pulse.insert(output_name);
            }
        }
        return state; // Return the state that was sent to the module's outputs.
    }
};

class FlipFlop : public Module 
{
public:
    FlipFlop(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs) {};

protected: 
    bool on = false; 

    bool receive_pulse(const std::string& sender_name, Pulse p) override
    {
        assert(input_pulses.contains(sender_name));
        if (p == Pulse::High) {
            return false; 
        } else {
            state = on ? Pulse::Low : Pulse::High; 
            on = !on;
            return true;
        }
    }

    void update_state() override {};
};

class Conjunction : public Module 
{
public:
    Conjunction(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs) {};

protected:
    bool receive_pulse(const std::string& sender_name, Pulse p) override
    {
        assert(input_pulses.contains(sender_name));
        input_pulses.at(sender_name) = p; 
        return true;
    }  

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
    Broadcast(const std::string& name, const std::vector<std::string>& outputs) : Module(name, outputs) {};

    bool receive_pulse(const std::string& sender_name, Pulse p) override
    {
        assert(sender_name == "button");
        state = p; 
        return true; 
    }

    void update_state() override {};
};


Network::Network(const std::vector<std::string>& lines)
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
            insert_module(std::make_unique<Broadcast>(lhs, dest_mods));
        } else {
            lhs = lhs.substr(1, lhs.size()); 
            if (module_sym == '%') {
                insert_module(std::make_unique<FlipFlop>(lhs, dest_mods));
            } else {
                insert_module(std::make_unique<Conjunction>(lhs, dest_mods));
            }
        }
    }
}

void Network::insert_module(std::unique_ptr<Module> mod) 
{
    if (modules.contains(mod->name)) {
        throw std::invalid_argument("Network::insert_module: Duplicate module"); 
    }
    modules.insert({mod->name, std::move(mod)}); 
    update_connections();
}

void Network::update_connections() 
{
    for (auto& [input_name, input_mod] : modules) {
        for (const std::string& output_name : input_mod->outputs) {
            if (!modules.contains(output_name)) {
                continue;
            }
            Module *output_mod = modules.at(output_name).get();
            assert(output_mod);
            output_mod->input_pulses.insert_or_assign(input_name, Pulse::Low);
        }
    }
}

int64_t Network::push_button(int n)
{
    int64_t low_pulses = 0; 
    int64_t high_pulses = 0; 

    for (int64_t i = 0; i < n; ++i) {
        low_pulses += 1; 
        std::queue<Module*> queue; 
        queue.push(modules.at("broadcaster").get());

        while (!queue.empty()) {
            Module *mod = queue.front();
            assert(mod);
            queue.pop();

            std::set<std::string> will_send_pulse; 
            Pulse sent_state = mod->forward_pulse(*this, will_send_pulse); 
            if (sent_state == Pulse::Low) {
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

int64_t find_lowest_rx(const std::vector<std::string>& lines)
{
    Network net {lines};
    auto module_before_rx_kv = std::find_if(net.modules.cbegin(), net.modules.cend(), [](const auto& kv) -> bool {
        auto found = std::find(kv.second->outputs.cbegin(), kv.second->outputs.cend(), "rx"); 
        return found != kv.second->outputs.cend(); 
    });

    assert(module_before_rx_kv != net.modules.cend()); 
    Module *mod_before_rx = module_before_rx_kv->second.get();
    assert(mod_before_rx);

    if (!dynamic_cast<Conjunction*>(mod_before_rx)) {
        throw std::invalid_argument("find_lowest_rx: heuristic does not work for given input since the input module of rx is not a conjunction");
    }

    auto next_before_rx_kv = std::find_if(std::next(module_before_rx_kv), net.modules.cend(), [](const auto& kv) -> bool {
        auto found = std::find(kv.second->outputs.cbegin(), kv.second->outputs.cend(), "rx"); 
        return found != kv.second->outputs.cend(); 
    });
    if (next_before_rx_kv != net.modules.cend()) {
        throw std::invalid_argument("find_lowest_rx: heuristic does not work for given input since rx has more than one input module");

    }

    std::unordered_map<std::string, std::vector<int64_t>> high_cycles;
    for (const auto& name_pulse : mod_before_rx->input_pulses) {
        high_cycles.insert({name_pulse.first, std::vector<int64_t>{}});
    }

    auto get_cycle = [](const std::vector<int64_t>& nums) -> std::optional<int64_t> {
        if (nums.size() < 2) {
            return {};
        }
        for (std::size_t i = 2; i < nums.size(); ++i) {
            int64_t prev_diff = nums.at(i - 1) -  nums.at(i - 2);
            int64_t diff = nums.at(i) -  nums.at(i - 1);
            if (diff == prev_diff) {
                return nums.at(i - 2);
            }
        }
        return {};
    };

    bool cycles_found = false; 
    for (int64_t i = 1; !cycles_found; ++i) {
        std::queue<Module*> queue; 
        queue.push(net.modules.at("broadcaster").get());

        bool got_to_rx = false; 
        while (!queue.empty()) {
            Module *mod = queue.front();
            queue.pop();

            std::set<std::string> will_send_pulse; 
            mod->forward_pulse(net, will_send_pulse); 

            if (!got_to_rx && mod->outputs.at(0) == "rx") {
                std::size_t found_n_cycles = 0; 
                for (const auto& [name, pulse] : mod->input_pulses) {
                    if (get_cycle(high_cycles.at(name))) {
                        ++found_n_cycles;
                    } else if (pulse == Pulse::High) { 
                        high_cycles.at(name).push_back(i); 
                    }
                }
                if (found_n_cycles == mod->input_pulses.size()) { 
                    // Found cycles for each input module of "rx"'s input module where they turn "high"
                    cycles_found = true;
                    break; 
                }
                got_to_rx = true;
            }
            for (const std::string& out_name : will_send_pulse) {
                assert(net.modules.contains(out_name));
                queue.push(net.modules.at(out_name).get());
            }
        }
    }

    std::vector<int64_t> cycles; 
    for (const auto& [name, vec] : high_cycles) {
        int64_t cycle = get_cycle(vec).value(); 
        cycles.push_back(cycle);
    }

    // We use the lcm analogous to day 8 (we want to find the smallest number where all cycles coincide/meet.) 
    auto lcm = [](const int64_t a, const int64_t b) -> int64_t {
        return std::lcm(a, b); 
    };
    int64_t min_button_presses = std::accumulate(cycles.begin(), cycles.end(), 1ll, lcm); 
    return min_button_presses;
}

int64_t part_one(const std::vector<std::string>& lines)
{
    Network network {lines};
    return network.push_button(1000);
}

int64_t part_two(const std::vector<std::string>& lines)
{
    Network network {lines};
    return find_lowest_rx(lines);
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