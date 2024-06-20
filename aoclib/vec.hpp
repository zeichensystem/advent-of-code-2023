#pragma once

#include <memory>
#include "hash.hpp"

namespace aocutil 
{

template<typename T>
struct Vec2 
{
    T x, y; 

    Vec2 operator+(const Vec2& v) const 
    {
        return Vec2{.x = x + v.x, .y = y + v.y};
    }

    Vec2 operator-(const Vec2& v) const 
    {
        return Vec2{.x = x - v.x, .y = y - v.y};
    }
    
    bool operator==(const Vec2& v) const = default;
};

}

template<typename T>
struct std::hash<aocutil::Vec2<T>>
{
    std::size_t operator()(const aocutil::Vec2<T>& v) const noexcept
    {
        std::size_t h = 0;
        aocutil::hash_combine(h, v.x, v.y);
        return h; 
    }
};

template<typename T>
inline std::ostream& operator<<(std::ostream&os, const aocutil::Vec2<T>& v) {
    return os << "(x: " << v.x << ", y: " << v.y << ")";
}