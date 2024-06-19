#include <memory>

namespace aocutil 
{

template<typename T>
struct Vec2 {
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
        std::size_t h1 = std::hash<T>{}(v.x); 
        std::size_t h2 = std::hash<T>{}(v.y); 
        return h1 ^ (h2 << 1); // cf. https://en.cppreference.com/w/cpp/utility/hash (last retrieved 2024-06-17)
    }
};