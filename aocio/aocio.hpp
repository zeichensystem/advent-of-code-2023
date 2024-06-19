#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <limits>
#include <cassert>
#include <optional>

#include "aocio.hpp"

#ifndef AOC_INPUT_PATH
#define AOC_INPUT_PATH ""
#endif

#ifndef AOC_INPUT_DIR
#define AOC_INPUT_DIR ""
#endif

namespace aocio 
{
bool file_getlines(std::string_view fname, std::vector<std::string>& lines);
void line_tokenise(const std::string& line, const std::string& delims, const std::string& preserved_delims, std::vector<std::string>& tokens);

int parse_num(const std::string &str);
int64_t parse_num_i64(const std::string& str);

inline void print_day() 
{
    std::string day_name {std::filesystem::path(AOC_INPUT_DIR).parent_path().filename()};
    
    if (day_name.size()) {
        day_name[0] = std::toupper(day_name[0]);
    }

    std::string debug_release;
    #ifdef NDEBUG
    debug_release = "Release";
    #else
    debug_release = "Debug";
    #endif

    std::cout << day_name << " (" << debug_release << ")\n";
}
}

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

template<typename RowType, typename ElemType>
class Grid;

// cf. on custom iterators: https://internalpointers.com/post/writing-custom-iterators-modern-cpp (last retrieved 2024-06-19)
template<typename RowType, typename ElemType, bool is_const>
struct GridColIterator 
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = int;
    using value_type        = ElemType;
    using pointer           = typename std::conditional_t<is_const, const ElemType*, ElemType*>;  
    using reference         = typename std::conditional_t<is_const, const ElemType&, ElemType&>;
    using parent_ptr_type   = typename std::conditional_t<is_const, const Grid<RowType, ElemType>*, Grid<RowType, ElemType>*>;

    // GridColIterator() = default;

    GridColIterator(int column, int row, parent_ptr_type parent) : col(column), current_row(row), parent(parent) 
    {
        assert(parent);
        if (parent == nullptr || !parent->pos_on_grid(col, row)) {
            ptr = nullptr;
        } else {
            ptr = &parent->rows.at(row).at(col);
        }
    };

    reference operator*() const {assert(ptr != nullptr); return *ptr; }
    pointer operator->() const {assert(ptr != nullptr); return ptr; }

    // Prefix:
    GridColIterator& operator++() 
    {
        assert(parent);
         if (current_row < parent->height() - 1 && current_row >= -1)  {
            ++current_row;
            ptr = &parent->rows.at(current_row).at(col);
         } else {
            ++current_row;
            ptr = nullptr;
         }
         return *this; 
    }  

    GridColIterator& operator--() 
    {
        assert(parent);
         if (current_row > 0 && current_row <= parent->height())  {
            --current_row;
            ptr = &parent->rows.at(current_row).at(col);
         } else {
            --current_row;
            ptr = nullptr;
         }
         return *this; 
    }  

    // Postfix: 
    GridColIterator operator++(int) { GridColIterator tmp = *this; ++(*this); return tmp;}
    GridColIterator operator--(int) { GridColIterator tmp = *this; --(*this); return tmp;}

    GridColIterator& operator+=(const difference_type& n) 
    {
        current_row += n;
        if (current_row < 0 || current_row >= parent->height()) {
            ptr = nullptr; 
        } else {
            ptr = &parent->at(col, current_row);
        }
        return *this;
    }

    GridColIterator& operator-=(const difference_type& n) {
        return *this += -n;
    }

    GridColIterator operator+(const difference_type& n) const
    {
        GridColIterator res = GridColIterator(col, current_row + n, parent);
        if (res.current_row < 0 || res.current_row >= parent->height()) {
            res.ptr = nullptr; 
        } else {
            res.ptr = &parent->at(col, res.current_row);
        }
        return res;
    }

    GridColIterator operator-(const difference_type& n) const {
        return *this + -n;
    }

    ElemType& operator[](difference_type n) const
    {
        int new_row = current_row - n;
        if (new_row < 0 || new_row >= parent->height()) {
            throw  std::out_of_range("GridIter: subscript out of range");
        }
        return parent->at(col, new_row);
    }

    difference_type operator-(const GridColIterator& other) const 
    {
        assert(parent == other.parent);
        assert(col == other.col); 
        return current_row - other.current_row;
    }

    auto operator<=>(const GridColIterator& rhs) const
    {
        assert(col == rhs.col);
        return current_row - rhs.current_row;
    }

    friend GridColIterator operator+(const difference_type& n, const GridColIterator& iter) 
    {
        return iter + n;
    }
    friend GridColIterator operator-(const difference_type& n, const GridColIterator& iter)
    {
        return iter - n; 
    }

    friend bool operator==(const GridColIterator& a, const GridColIterator& b) {assert(a.col == b.col && a.parent == b.parent); return a.current_row == b.current_row; };
    friend bool operator!=(const GridColIterator& a, const GridColIterator& b) {assert(a.col == b.col && a.parent == b.parent); return a.current_row != b.current_row;};  

private:
    int col; 
    int current_row;
    parent_ptr_type parent = nullptr; // ptr itself is const (and parent_ptr_type depending on is_const).
    pointer ptr; 
};


template<typename RowType, typename ElemType, bool is_const>
struct GridIterator 
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = int;
    using value_type        = ElemType;
    using pointer           = typename std::conditional_t<is_const, const ElemType*, ElemType*>;  
    using reference         = typename std::conditional_t<is_const, const ElemType&, ElemType&>;
    using parent_ptr_type   = typename std::conditional_t<is_const, const Grid<RowType, ElemType>*, Grid<RowType, ElemType>*>;

    // GridIterator() = default;

    GridIterator(int column, int row, parent_ptr_type parent) : current_col(column), current_row(row), parent(parent) 
    {
        assert(parent);
        if (parent == nullptr || !parent->pos_on_grid(current_col, current_row)) {
            ptr = nullptr;
        } else {
            ptr = &parent->rows.at(current_row).at(current_col);
        }
    };

    reference operator*() const {assert(ptr != nullptr); return *ptr; }
    pointer operator->() const {assert(ptr != nullptr); return ptr; }

    // Prefix:
    GridIterator& operator++() 
    {
        assert(parent);
        difference_type new_idx = 0; 
        if (current_col == -1 && current_row == -1) {
            new_idx = 0;
        } else {
            new_idx = current_col + current_row * parent->width() + 1;
        }
        current_col = new_idx % parent->width();
        current_row = new_idx / parent->width();

        if (parent->pos_on_grid(current_col, current_row)) {
            ptr = &parent->rows.at(current_row).at(current_col);
        } else {
            current_col = new_idx >= 0 ? parent->width() : -1;
            current_row = new_idx >= 0 ? parent->height() : -1;
            ptr = nullptr;
        }
         return *this;
    }  

    GridIterator& operator--() 
    {
        assert(parent);
        difference_type new_idx = 0; 
        if (current_col == parent->width() && current_row == parent->height()) {
            new_idx = parent->width() * parent->height() - 1; 
        } else {
            new_idx = current_col + current_row * parent->width() - 1;
        }
        current_col = new_idx % parent->width();
        current_row = new_idx / parent->width();
        
        if (parent->pos_on_grid(current_col, current_row)) {
            ptr = &parent->rows.at(current_row).at(current_col);
        } else {
            ptr = nullptr;
            current_col = new_idx >= 0 ? parent->width() : -1;
            current_row = new_idx >= 0 ? parent->height() : -1;
        }
         return *this;
    }  

    // Postfix: 
    GridIterator operator++(int) { GridIterator tmp = *this; ++(*this); return tmp;}
    GridIterator operator--(int) { GridIterator tmp = *this; --(*this); return tmp;}

    GridIterator& operator+=(const difference_type& n) 
    {
        assert(parent);
        difference_type new_idx = 0; 
        if (current_col == -1 && current_row == -1) {
            new_idx = n - 1;
        } else {
            new_idx = current_col + current_row * parent->width() + n;
        }
        current_col = new_idx % parent->width();
        current_row = new_idx / parent->width();

        if (parent->pos_on_grid(current_col, current_row)) {
            ptr = &parent->rows.at(current_row).at(current_col);
        } else {
            current_col = new_idx >= 0 ? parent->width() : -1;
            current_row = new_idx >= 0 ? parent->height() : -1;
            ptr = nullptr;
        }
        return *this;
    }

    GridIterator& operator-=(const difference_type& n) {
        return *this += -n;
    }

    GridIterator operator+(const difference_type& n) const
    {
        assert(parent);
        GridIterator res = GridIterator(current_col, current_row, parent);
        difference_type new_idx = 0; 
        if (current_col == -1 && current_row == -1) {
            new_idx = 0;
        } else {
            new_idx = current_col + current_row * parent->width() + n;
        }
        res.current_col = new_idx % parent->width();
        res.current_row = new_idx / parent->width();
    
        if (parent->pos_on_grid(res.current_col, res.current_row)) {
            res.ptr = &parent->rows.at(res.current_row).at(res.current_col);
        } else {
            res.ptr = nullptr;
            res.current_col = new_idx >= 0 ? parent->width() : -1;
            res.current_row = new_idx >= 0 ? parent->height() : -1;
        }
 
        return res;
    }

    GridIterator operator-(const difference_type& n) const {
        return *this + -n;
    }

    ElemType& operator[](difference_type n) const
    {
        difference_type new_idx = current_col + current_row * parent->width() + n;
        int new_col = new_idx % parent->width();
        int new_row = new_idx / parent->width();
        if (!parent->pos_on_grid(new_col, new_row)) {
            throw std::out_of_range("Grid: subscript out of range.");
        }
        return parent->at(new_col, new_row);
    }

    difference_type operator-(const GridIterator& other) const 
    {
        assert(parent == other.parent);
        difference_type this_idx = current_col + current_row * parent->width(); 
        difference_type other_idx = other.current_col + other.current_row * parent->width(); 
        return this_idx - other_idx; 
    }

    auto operator<=>(const GridIterator& rhs) const
    {
        assert(rhs.parent == parent);
        difference_type this_idx = current_col + current_row * parent->width(); 
        difference_type other_idx = rhs.current_col + rhs.current_row * parent->width(); 
        return this_idx - other_idx;
    }

    friend GridIterator operator+(const difference_type& n, const GridIterator& iter) 
    {
        return iter + n;
    }

    friend GridIterator operator-(const difference_type& n, const GridIterator& iter)
    {
        return iter - n;
    }

    friend bool operator==(const GridIterator& a, const GridIterator& b) {assert(a.parent == b.parent); return a.ptr == b.ptr;};
    friend bool operator!=(const GridIterator& a, const GridIterator& b) {assert(a.parent == b.parent); return a.ptr != b.ptr;};  

private:
    int current_col = 0; 
    int current_row = 0;
    parent_ptr_type parent = nullptr;
    pointer ptr = nullptr; 
};


template<typename RowType, typename ElemType>
class Grid 
{
    std::vector<RowType> rows;
    int width_ = 0, height_ = 0; 

    using GridColIteratorMut = GridColIterator<RowType, ElemType, false>;
    using GridColIteratorConst = GridColIterator<RowType, ElemType, true>;
    using GridIteratorMut = GridIterator<RowType, ElemType, false>; 
    using GridIteratorConst = GridIterator<RowType, ElemType, true>;

    friend GridColIteratorMut;
    friend GridColIteratorConst;
    friend GridIteratorMut;
    friend GridIteratorConst;

public: 
    Grid() = default; 

    Grid(const std::vector<RowType>& rows) 
    {
        if (!rows.size()) {
            return;
        }
        width_ = rows.at(0).size();
        height_ = rows.size();
        for (const auto& row : rows) {
            assert(std::ssize(row) == width_); 
        }
    }

    void push_row(const RowType& row) 
    {
        if (width_ != 0 && !(std::ssize(row) == width_)) {
            throw  std::out_of_range("Grid add_row: row size does not match");
        } else {
            width_ = std::ssize(row);
        }
        rows.push_back(row);
        ++height_;
        if (width_ == 0 || height_ == 0) {
            throw std::runtime_error("Grid: Tried to push empty row.");
        }
    }

    std::optional<ElemType> try_get(int x, int y) const 
    {
        if (!pos_on_grid(x, y)) {
            return {};
        }
        return rows.at(y).at(x);
    }
    std::optional<ElemType> try_get(const Vec2<int>& pos) const {
        return try_get(pos.x, pos.y); 
    }

    ElemType get(int x, int y) const 
    {
        if (!pos_on_grid(x, y)) {
            throw std::out_of_range("Grid get: invalid position");
        }
        return rows.at(y).at(x);
    }
    ElemType get(const Vec2<int>& pos) const {
        return get(pos.x, pos.y);
    }

    void set(int x, int y, ElemType e) 
    {
        if (!pos_on_grid(x, y)) {
            throw std::out_of_range("Grid set: invalid position");
        }
        rows.at(y).at(x) = e;
    }
    void set(const Vec2<int>& pos, ElemType e) {
        set(pos.x, pos.y, e);
    }

    bool try_set(int x, int y, ElemType e) 
    {
        if (!pos_on_grid(x, y)) {
            return false;
        }
        set(x, y);
        return true;
    }
    bool try_set(const Vec2<int>& pos, ElemType e) {
        return try_set(pos.x, pos.y);
    }

    ElemType& at(int x, int y) 
    {
        if (!pos_on_grid(x, y)) {
            throw std::out_of_range("Grid at: invalid position");
        }
        return rows.at(y).at(x);
    }
    ElemType& at(const Vec2<int>& pos) {
        return at(pos.x, pos.y);
    }

    const ElemType& at(int x, int y) const
    {
        if (!pos_on_grid(x, y)) {
            throw std::out_of_range("Grid at: invalid position");
        }
        return rows.at(y).at(x);
    }
    const ElemType& at(const Vec2<int>& pos) const {
        return at(pos.x, pos.y);
    }

    RowType& operator[](int row) 
    {
        if (row < 0 || row >= height_) {
            throw std::out_of_range("Grid: invalid row");
        } else {
            return rows.at(row);
        }
    }

    bool pos_on_grid(int x, int y) const {
        return x >= 0 && x < width_ && y >= 0 && y < height_; 
    }
    bool pos_on_grid(const Vec2<int>& pos) const {
        return pos_on_grid(pos.x, pos.y);
    }

    int height() const {
        return height_;
    }
    int width() const {
        return width_;
    }

    GridIteratorMut begin() {
        return GridIteratorMut(0, 0, this);
    }
    GridIteratorMut end() {
        return GridIteratorMut(width(), height(), this);
    }

    GridIteratorConst cbegin() {
        return GridIteratorConst(0, 0, this);
    }
    GridIteratorConst cend() {
        return GridIteratorConst(width(), height(), this);
    }

    GridColIteratorMut begin_col(int col) {
        return GridColIteratorMut(col, 0, this);
    }

    GridColIteratorMut end_col(int col) {
        return GridColIteratorMut(col, height_, this);
    }

    GridColIteratorConst cbegin_col(int col) const {
        return GridColIteratorConst(col, 0, this);
    }

    GridColIteratorConst cend_col(int col) const {
        return GridColIteratorConst(col, height_, this);
    }

    typename RowType::iterator begin_row(int row) {
        return rows.at(row).begin();
    }

    typename RowType::iterator end_row(int row) {
        return rows.at(row).end();
    }

    typename RowType::const_iterator cbegin_row(int row) const {
        return rows.at(row).cbegin();
    }

    typename RowType::const_iterator cend_row(int row) const {
        return rows.at(row).cend();
    }

    friend std::ostream& operator<<(std::ostream& os, const Grid<RowType, ElemType>& g) 
    {
        for (int row_n = 0; row_n < g.height(); ++row_n) {
            for (auto elem = g.cbegin_row(row_n); elem != g.cend_row(row_n); ++elem) {
                os << *elem; 
            }
            os << "\n";
        }
        return os;
    }
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