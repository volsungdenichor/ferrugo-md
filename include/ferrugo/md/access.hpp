#pragma once

#include <ferrugo/md/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md
{

namespace detail
{

struct extents_fn
{
    auto operator()(const bounds_base_t& item) const -> std::optional<extents_base_t>
    {
        if (item.lower == item.upper)
        {
            return {};
        }
        return extents_base_t{ item.lower, item.upper - 1 };
    }

    template <std::size_t D>
    auto operator()(const bounds_t<D>& item) const -> std::optional<extents_t<D>>
    {
        extents_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            const auto res = (*this)(item[d]);
            if (!res)
            {
                return {};
            }
            result[d] = *res;
        }
        return result;
    }

    auto operator()(const dim_base_t& item) const -> std::optional<extents_base_t>
    {
        if (item.size == 0)
        {
            return {};
        }
        return extents_base_t{ 0, item.size - 1 };
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> std::optional<extents_t<D>>
    {
        extents_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            const auto res = (*this)(item[d]);
            if (!res)
            {
                return {};
            }
            result[d] = *res;
        }
        return result;
    }
};

struct size_fn
{
    auto operator()(const bounds_base_t& item) const -> location_base_t
    {
        return item.upper - item.lower;
    }

    template <std::size_t D>
    auto operator()(const bounds_t<D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_base_t item) const -> location_base_t
    {
        return item.size;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> location_t<D>
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
    auto operator()(const dim_base_t item) const -> location_base_t
    {
        return item.stride;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> location_t<D>
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
    auto operator()(const bounds_base_t& item) const -> bounds_base_t
    {
        return item;
    }

    template <std::size_t D>
    auto operator()(const bounds_t<D>& item) const -> bounds_t<D>
    {
        return item;
    }

    auto operator()(const dim_base_t& item) const -> bounds_base_t
    {
        return bounds_base_t{ 0, item.size };
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> bounds_t<D>
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
    auto operator()(const bounds_base_t& item, const location_base_t& loc) const -> bool
    {
        return item.lower <= loc && loc < item.upper;
    }

    template <std::size_t D>
    auto operator()(const bounds_t<D>& item, const location_t<D>& loc) const -> bool
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

    auto operator()(const dim_base_t& item, const location_base_t& loc) const -> bool
    {
        return 0 <= loc && loc < item.size;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item, const location_t<D>& loc) const -> bool
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

struct flat_offset_fn
{
    auto operator()(const dim_base_t& item, const location_base_t& loc) const -> flat_offset_t
    {
        return loc * item.stride;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item, const location_t<D>& loc) const -> flat_offset_t
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
    auto operator()(const bounds_base_t& item) const -> volume_t
    {
        return size_fn{}(item);
    }

    template <std::size_t D>
    auto operator()(const bounds_t<D>& item) const -> volume_t
    {
        volume_t result = 1;
        for (std::size_t d = 0; d < D; ++d)
        {
            result *= (*this)(item[d]);
        }
        return result;
    }

    auto operator()(const dim_base_t& item) const -> volume_t
    {
        return item.size;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> volume_t
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

static constexpr inline auto extents = adaptable<detail::extents_fn>{};
static constexpr inline auto size = adaptable<detail::size_fn>{};
static constexpr inline auto stride = adaptable<detail::stride_fn>{};
static constexpr inline auto bounds = adaptable<detail::bounds_fn>{};
static constexpr inline auto contains = adaptable<detail::contains_fn>{};
static constexpr inline auto volume = adaptable<detail::volume_fn>{};
static constexpr inline auto flat_offset = detail::flat_offset_fn{};

}  // namespace md
}  // namespace ferrugo
