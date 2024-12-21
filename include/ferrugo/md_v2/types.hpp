#pragma once

#include <array>
#include <cstddef>
#include <ostream>

namespace ferrugo
{
namespace md_v2
{

namespace detail
{

template <class T, std::size_t D>
struct array_base : public std::array<T, D>
{
    using base_t = std::array<T, D>;
    using value_type = typename base_t::value_type;

    array_base()
    {
        std::fill(base_t::begin(), base_t::end(), value_type{});
    }

    template <class... Tail>
    array_base(value_type head, Tail... tail) : base_t{ { head, tail... } }
    {
        static_assert(sizeof...(tail) + 1 == D, "all values required to be set");
    }

    friend std::ostream& operator<<(std::ostream& os, const array_base& item)
    {
        os << "(";
        for (std::size_t d = 0; d < D; ++d)
        {
            if (d != 0)
            {
                os << " ";
            }
            os << item[d];
        }
        os << ")";
        return os;
    }
};

template <std::size_t D>
struct location_base_t;

template <std::size_t D>
struct bounds_base_t;

template <std::size_t D>
struct dim_base_t;

template <std::size_t D = 1>
using location_t = typename location_base_t<D>::type;

template <std::size_t D = 1>
using bounds_t = typename bounds_base_t<D>::type;

template <std::size_t D = 1>
using dim_t = typename dim_base_t<D>::type;

template <>
struct location_base_t<1>
{
    using type = std::ptrdiff_t;
};

template <std::size_t D>
struct location_base_t
{
    using type = array_base<location_t<1>, D>;
};

template <>
struct bounds_base_t<1>
{
    struct type
    {
        location_t<1> lower = 0;
        location_t<1> upper = 0;

        friend bool operator==(const type& lhs, const type& rhs)
        {
            return std::tie(lhs.lower, lhs.upper) == std::tie(rhs.lower, rhs.upper);
        }

        friend bool operator!=(const type& lhs, const type& rhs)
        {
            return !(lhs == rhs);
        }

        friend std::ostream& operator<<(std::ostream& os, const type& item)
        {
            return os << "(" << item.lower << " " << item.upper << ")";
        }
    };
};

template <std::size_t D>
struct bounds_base_t
{
    using type = array_base<bounds_t<1>, D>;
};

template <>
struct dim_base_t<1>
{
    struct type
    {
        location_t<1> size = 0;
        location_t<1> stride = 0;
        location_t<1> min = 0;

        friend bool operator==(const type& lhs, const type& rhs)
        {
            return std::tie(lhs.size, lhs.stride, lhs.min) == std::tie(rhs.size, rhs.stride, rhs.min);
        }

        friend bool operator!=(const type& lhs, const type& rhs)
        {
            return !(lhs == rhs);
        }

        friend std::ostream& operator<<(std::ostream& os, const type& item)
        {
            return os << "(" << item.size << " " << item.stride << " " << item.min << ")";
        }
    };
};

template <std::size_t D>
struct dim_base_t
{
    using type = array_base<dim_t<1>, D>;
};

}  // namespace detail

using flat_offset_t = std::ptrdiff_t;
using volume_t = std::ptrdiff_t;

using detail::bounds_t;
using detail::dim_t;
using detail::location_t;

}  // namespace md_v2
}  // namespace ferrugo
