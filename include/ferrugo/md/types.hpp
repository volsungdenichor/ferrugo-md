#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>

namespace ferrugo
{
namespace md
{

template <class... Args>
auto str(Args&&... args) -> std::string
{
    std::stringstream ss;
    (ss << ... << std::forward<Args>(args));
    return ss.str();
}

using byte = std::uint8_t;

template <class T, std::size_t D>
struct tuple_t : public std::array<T, D>
{
    using base_t = std::array<T, D>;
    using value_type = typename base_t::value_type;

    tuple_t()
    {
        std::fill(base_t::begin(), base_t::end(), value_type{});
    }

    template <
        std::size_t D_ = D,
        class... Args,
        std::enable_if_t<D_ == 1 && std::is_constructible_v<value_type, Args...>, int> = 0>
    tuple_t(Args&&... args) : base_t{}
    {
        (*this)[0] = value_type(std::forward<Args>(args)...);
    }

    template <std::size_t D_ = D, std::enable_if_t<D_ == 1, int> = 0>
    operator value_type() const
    {
        return (*this)[0];
    }

    template <class... Tail>
    tuple_t(value_type head, Tail... tail) : base_t{ { head, tail... } }
    {
        static_assert(sizeof...(tail) + 1 == D, "all values required to be set");
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto erase(std::size_t i) const -> tuple_t<T, D - 1>
    {
        tuple_t<T, D - 1> result;
        for (std::size_t d = 0; d < i; ++d)
        {
            result[d + 0] = (*this)[d];
        }
        for (std::size_t d = i + 1; d < D_; ++d)
        {
            result[d - 1] = (*this)[d];
        }
        return result;
    }

    auto insert(std::size_t pos, T item) const -> tuple_t<T, D + 1>
    {
        tuple_t<T, D + 1> result;
        auto iter = std::copy(this->begin(), this->begin() + pos, result.begin());
        *iter++ = std::move(item);
        std::copy(this->begin() + pos, this->end(), iter);
        return result;
    }

    auto append(T item) const -> tuple_t<T, D + 1>
    {
        tuple_t<T, D + 1> result;
        std::copy(this->begin(), this->end(), result.begin());
        result[D] = std::move(item);
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const tuple_t& item)
    {
        os << "[";
        for (std::size_t d = 0; d < D; ++d)
        {
            if (d != 0)
            {
                os << " ";
            }
            os << item[d];
        }
        os << "]";
        return os;
    }
};

using location_base_t = std::ptrdiff_t;
using size_base_t = location_base_t;

using flat_offset_t = std::ptrdiff_t;
using volume_t = std::ptrdiff_t;

struct extents_base_t
{
    location_base_t min;
    location_base_t max;

    extents_base_t() = default;

    extents_base_t(location_base_t min, location_base_t max) : min(min), max(max)
    {
    }

    friend bool operator==(const extents_base_t& lhs, const extents_base_t& rhs)
    {
        return std::tie(lhs.min, lhs.max) == std::tie(rhs.min, rhs.max);
    }

    friend bool operator!=(const extents_base_t& lhs, const extents_base_t& rhs)
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const extents_base_t& item)
    {
        return os << "{"
                  << ":min " << item.min << " "
                  << ":max " << item.max << "}";
    }
};

struct bounds_base_t
{
    location_base_t lower;
    location_base_t upper;

    bounds_base_t() = default;

    bounds_base_t(location_base_t lo, location_base_t up) : lower(lo), upper(up)
    {
    }

    friend bool operator==(const bounds_base_t& lhs, const bounds_base_t& rhs)
    {
        return std::tie(lhs.lower, lhs.upper) == std::tie(rhs.lower, rhs.upper);
    }

    friend bool operator!=(const bounds_base_t& lhs, const bounds_base_t& rhs)
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const bounds_base_t& item)
    {
        return os << "{"
                  << ":lower " << item.lower << " "
                  << ":upper " << item.upper << "}";
    }
};

struct dim_base_t
{
    location_base_t size;
    location_base_t stride;

    dim_base_t() = default;

    dim_base_t(location_base_t size, location_base_t stride) : size(size), stride(stride)
    {
    }

    friend bool operator==(const dim_base_t& lhs, const dim_base_t& rhs)
    {
        return std::tie(lhs.size, lhs.stride) == std::tie(rhs.size, rhs.stride);
    }

    friend bool operator!=(const dim_base_t& lhs, const dim_base_t& rhs)
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const dim_base_t& item)
    {
        return os << "{"
                  << ":size " << item.size << " "
                  << ":stride " << item.stride << "}";
    }
};

struct slice_base_t
{
    using value_type = std::optional<location_base_t>;

    value_type start;
    value_type stop;
    value_type step;

    slice_base_t(value_type start = {}, value_type stop = {}, value_type step = {}) : start(start), stop(stop), step(step)
    {
    }

    friend bool operator==(const slice_base_t& lhs, const slice_base_t& rhs)
    {
        return std::tie(lhs.start, lhs.stop, lhs.step) == std::tie(rhs.start, rhs.stop, rhs.step);
    }

    friend bool operator!=(const slice_base_t& lhs, const slice_base_t& rhs)
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const slice_base_t& item)
    {
        if (item.start)
        {
            os << *item.start;
        }
        os << ":";
        if (item.stop)
        {
            os << *item.stop;
        }
        os << ":";
        if (item.step)
        {
            os << *item.step;
        }
        return os;
    }
};

template <std::size_t D = 1>
using location_t = tuple_t<location_base_t, D>;

template <std::size_t D = 1>
using size_t = tuple_t<size_base_t, D>;

template <std::size_t D = 1>
using extents_t = tuple_t<extents_base_t, D>;

template <std::size_t D = 1>
using bounds_t = tuple_t<bounds_base_t, D>;

template <std::size_t D = 1>
using dim_t = tuple_t<dim_base_t, D>;

template <std::size_t D = 1>
using slice_t = tuple_t<slice_base_t, D>;

static constexpr inline auto _ = slice_base_t::value_type{};

inline auto at(location_base_t loc) -> slice_base_t
{
    return loc == -1  //
               ? slice_base_t{ -1, _, _ }
               : slice_base_t{ loc, loc + 1, _ };
}

}  // namespace md
}  // namespace ferrugo
