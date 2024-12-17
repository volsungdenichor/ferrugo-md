#pragma once

#include <array>
#include <cstddef>
#include <iostream>

namespace ferrugo
{

namespace md
{

using index_t = std::ptrdiff_t;
using flat_offset_t = std::ptrdiff_t;
using volume_t = std::ptrdiff_t;

template <std::size_t D>
struct location_t : std::array<index_t, D>
{
    using base_t = std::array<index_t, D>;

    using base_t::base_t;

    location_t()
    {
        std::fill(base_t::begin(), base_t::end(), 0);
    }

    template <class... Tail>
    location_t(index_t head, Tail... tail) : base_t{ { head, tail... } }
    {
        static_assert(sizeof...(tail) + 1 == D, "all values required to be set");
    }

    friend std::ostream& operator<<(std::ostream& os, const location_t& item)
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

struct extent_base_t
{
    index_t lower;
    index_t upper;
};

template <std::size_t D>
struct extent_t : std::array<extent_base_t, D>
{
    using base_t = std::array<extent_base_t, D>;
    using base_t::base_t;

    friend std::ostream& operator<<(std::ostream& os, const extent_t& item)
    {
        os << "[";
        for (std::size_t d = 0; d < D; ++d)
        {
            if (d != 0)
            {
                os << " ";
            }
            os << item[d].lower << ":" << item[d].upper;
        }
        os << "]";
        return os;
    }
};

}  // namespace md

}  // namespace ferrugo
