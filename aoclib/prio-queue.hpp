#pragma once

#include <map>
#include <unordered_map>
#include <optional>

namespace aocutil 
{
template<typename T, typename PrioType = int>
class PrioQueue 
{
private:
    std::multimap<PrioType, T> prio_to_elem; 
    std::unordered_map<T, typename decltype(prio_to_elem)::iterator> elem_to_prio; // Necessary so we don't have to do linear search when updating an element's priority.

public:
    void insert(const T& elem, const PrioType& priority)
    {
        if (elem_to_prio.contains(elem)) {
            throw std::invalid_argument("PrioQueue insert: Element already in queue"); 
        }
        auto it = prio_to_elem.insert({priority, elem});
        assert(it != prio_to_elem.end() && !elem_to_prio.contains(it->second));
        elem_to_prio.insert({elem, it}); 
    } 

    void update_prio(const T& elem, const PrioType& new_priority)
    {
        if (!elem_to_prio.contains(elem)) {
            throw std::out_of_range("PrioQueue update_prio: Element not in queue.");
        }
        auto it = elem_to_prio.at(elem); 
        assert(it != prio_to_elem.end());
        elem_to_prio.erase(elem);
        prio_to_elem.erase(it);
        insert(elem, new_priority); // Re-insert elem at the new priority.
    }

    void insert_or_update(const T& elem, const PrioType& prio)
    {
        if (contains(elem)) {
            update_prio(elem, prio); 
        } else {
            insert(elem, prio);
        }
    }

    std::optional<T> extract_min()
    {
        auto min_elem = prio_to_elem.begin(); 
        if (min_elem == prio_to_elem.end()) { // Queue is empty.
            assert(empty());
            return {}; 
        }
        T elem = min_elem->second; 
        prio_to_elem.erase(min_elem);
        elem_to_prio.erase(elem); 
        return elem;
    }

    bool contains(const T& elem) const {
        return elem_to_prio.contains(elem); 
    }

    std::size_t size() const {
        return elem_to_prio.size();
    }

    bool empty() const 
    {
        const bool empt = prio_to_elem.size() == 0; 
        if (empt) {
            assert(elem_to_prio.size() == 0);
        }
        return empt;
    }
};
}