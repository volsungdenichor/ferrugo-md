#pragma once

#include <ferrugo/md_v3/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md_v3
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
        return item.stride > 0 ? extents_base_t{ item.min, item.min + item.size - 1 }
                               : extents_base_t{ item.min, item.min + item.size + 1 };
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
        return bounds_base_t{ item.min, item.min + item.size };
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
        return item.min <= loc && loc < (item.min + item.size);
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

struct offset_fn
{
    auto operator()(const dim_base_t& item, const location_base_t& loc) const -> flat_offset_t
    {
        // return (loc - item.min) * item.stride;
        return item.min + loc * item.stride;
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

struct apply_slice_fn
{
    auto operator()(const dim_base_t& dim, const slice_base_t& slice) const -> dim_base_t
    {
        const auto step = slice.step.value_or(1);

        if (step == 0)
        {
            throw std::runtime_error{ "step cannot be zero" };
        }
        static const auto ensure_non_negative = [](location_base_t v) { return std::max(location_base_t(0), v); };
        const auto apply_size = [&](location_base_t v) { return v < 0 ? v + dim.size : v; };
        const auto clamp = [&](location_base_t v, location_base_t shift)
        { return std::max(location_base_t(shift), std::min(v, dim.size + shift)); };

        const auto [size, start]  //
            = step > 0            //
                  ? std::invoke(
                      [&]() -> std::tuple<location_base_t, location_base_t>
                      {
                          const auto start = clamp(apply_size(slice.start.value_or(0)), 0);
                          const auto stop = clamp(apply_size(slice.stop.value_or(dim.size)), 0);
                          const auto size = ensure_non_negative((stop - start + step - 1) / step);
                          return std::tuple{ size, start };
                      })
                  : std::invoke(
                      [&]() -> std::tuple<location_base_t, location_base_t>
                      {
                          const auto start = clamp(apply_size(slice.start.value_or(dim.size)), -1);
                          const auto stop = clamp(apply_size(slice.stop.value_or(0)), -1);
                          const auto size = ensure_non_negative((stop - start + step + 1) / step);
                          return std::tuple{ size, start };
                      });
        return dim_base_t{ std::min(size, dim.size), dim.stride * step, dim.min + dim.stride * start };
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& dim, const slice_t<D>& slice) const -> dim_t<D>
    {
        dim_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(dim[d], slice[d]);
        }
        return result;
    }
};

}  // namespace detail

static constexpr inline auto extents = detail::extents_fn{};
static constexpr inline auto size = detail::size_fn{};
static constexpr inline auto stride = detail::stride_fn{};
static constexpr inline auto bounds = detail::bounds_fn{};
static constexpr inline auto contains = detail::contains_fn{};
static constexpr inline auto volume = detail::volume_fn{};
static constexpr inline auto offset = detail::offset_fn{};
static constexpr inline auto apply_slice = detail::apply_slice_fn{};

}  // namespace md_v3
}  // namespace ferrugo
