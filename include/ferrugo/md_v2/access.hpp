#pragma once

#include <ferrugo/md_v2/types.hpp>
#include <iostream>

namespace ferrugo
{
namespace md_v2
{

namespace detail
{

struct min_fn
{
    auto operator()(const bounds_t<1>& item) const -> location_t<1>
    {
        return item.lower;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.min;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct max_fn
{
    auto operator()(const bounds_t<1>& item) const -> location_t<1>
    {
        return item.upper - 1;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.min + item.size - 1;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct lower_fn
{
    auto operator()(const bounds_t<1>& item) const -> location_t<1>
    {
        return item.lower;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.min;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct upper_fn
{
    auto operator()(const bounds_t<1>& item) const -> location_t<1>
    {
        return item.upper;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.min + item.size;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct size_fn
{
    auto operator()(const bounds_t<1>& item) const -> location_t<1>
    {
        return item.upper - item.lower;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.size;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct stride_fn
{
    auto operator()(const dim_t<1>& item) const -> location_t<1>
    {
        return item.stride;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct bounds_fn
{
    auto operator()(const bounds_t<1>& item) const -> bounds_t<1>
    {
        return item;
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> bounds_t<D>
    {
        return item;
    }

    auto operator()(const dim_t<1>& item) const -> bounds_t<1>
    {
        return bounds_t<1>{ item.min, item.min + item.size };
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> bounds_t<D>
    {
        bounds_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }
};

struct contains_fn
{
    auto operator()(const bounds_t<1>& item, const location_t<1>& loc) const -> bool
    {
        return lower_fn{}(item) <= loc && loc < upper_fn{}(item);
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item, const array_base<location_t, D>& loc) const -> bool
    {
        for (std::size_t d = 0; d < D; ++d)
        {
            if (!(*this)(item[d], loc[d]))
            {
                return false;
            }
        }
        return true;
    }

    auto operator()(const dim_t<1>& item, const location_t<1>& loc) const -> bool
    {
        return lower_fn{}(item) <= loc && loc < upper_fn{}(item);
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item, const array_base<location_t, D>& loc) const -> bool
    {
        for (std::size_t d = 0; d < D; ++d)
        {
            if (!(*this)(item[d], loc[d]))
            {
                return false;
            }
        }
        return true;
    }
};

struct offset_fn
{
    auto operator()(const dim_t<1>& item, const location_t<1>& loc) const -> flat_offset_t
    {
        return item.min + item.stride * loc;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item, const array_base<location_t, D>& loc) const -> flat_offset_t
    {
        flat_offset_t result = 0;
        for (std::size_t d = 0; d < D; ++d)
        {
            result += (*this)(item[d], loc[d]);
        }
        return result;
    }
};

struct volume_fn
{
    auto operator()(const bounds_t<1>& item) const -> volume_t
    {
        return size_fn{}(item);
    }

    template <std::size_t D>
    auto operator()(const array_base<bounds_t, D>& item) const -> volume_t
    {
        volume_t result = 1;
        for (std::size_t d = 0; d < D; ++d)
        {
            result *= (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_t<1>& item) const -> volume_t
    {
        return item.size;
    }

    template <std::size_t D>
    auto operator()(const array_base<dim_t, D>& item) const -> volume_t
    {
        volume_t result = 1;
        for (std::size_t d = 0; d < D; ++d)
        {
            result *= (*this)(item[d]);
        }
        return result;
    }
};

}  // namespace detail

static constexpr inline auto min = detail::min_fn{};
static constexpr inline auto max = detail::max_fn{};
static constexpr inline auto lower = detail::lower_fn{};
static constexpr inline auto upper = detail::upper_fn{};
static constexpr inline auto size = detail::size_fn{};
static constexpr inline auto stride = detail::stride_fn{};
static constexpr inline auto bounds = detail::bounds_fn{};
static constexpr inline auto contains = detail::contains_fn{};
static constexpr inline auto volume = detail::volume_fn{};
}  // namespace md_v2
}  // namespace ferrugo