#pragma once

#include <ferrugo/md_v2/types.hpp>
#include <functional>
#include <optional>

namespace ferrugo
{
namespace md_v2
{

namespace detail
{

template <std::size_t D>
struct slice_base_t;

template <std::size_t D = 1>
using slice_t = typename slice_base_t<D>::type;

template <>
struct slice_base_t<1>
{
    struct type
    {
        std::optional<location_t<1>> start = std::nullopt;
        std::optional<location_t<1>> stop = std::nullopt;
        std::optional<location_t<1>> step = std::nullopt;

        friend bool operator==(const type& lhs, const type& rhs)
        {
            return std::tie(lhs.start, lhs.stop, lhs.step) == std::tie(rhs.start, rhs.stop, rhs.step);
        }

        friend bool operator!=(const type& lhs, const type& rhs)
        {
            return !(lhs == rhs);
        }

        friend std::ostream& operator<<(std::ostream& os, const type& item)
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
};

template <std::size_t D>
struct slice_base_t
{
    using type = array_base<slice_t<1>, D>;
};

inline auto indices(const slice_t<1>& s, location_t<1> n) -> std::array<location_t<1>, 3>
{
    const auto step = s.step.value_or(1);
    const auto clamp = [&](location_t<1> v) { return std::max(location_t<1>(0), std::min(v, n)); };
    const auto adjust_val = [&](location_t<1> v) { return clamp(v >= 0 ? v : n + v); };
    if (step > 0)
    {
        const auto start = s.start ? adjust_val(*s.start) : 0;
        const auto stop = s.stop ? adjust_val(*s.stop) : n;
        return { start, stop, step };
    }
    if (step < 0)
    {
        const auto start = s.start ? adjust_val(*s.start) : n - 1;
        const auto stop = s.stop ? adjust_val(*s.stop) : -1;
        return { start, stop, step };
    }
    throw std::runtime_error{ "step cannot be zero" };
}

inline auto handle(const dim_t<1>& d, const slice_t<1>& s) -> dim_t<1>
{
    const auto [start, stop, step] = indices(s, d.size);
    const auto size = std::invoke(
        [&]() -> location_t<1>
        {
            location_t<1> res = 0;
            if (step > 0)
            {
                for (location_t<1> val = start; val < stop; val += step, ++res)
                {
                }
            }
            else
            {
                for (location_t<1> val = start; val > stop; val += step, ++res)
                {
                }
            }
            return res;
        });
    return { std::max<location_t<1>>(0, size), d.stride * step, step * -(d.min - start) };
}

}  // namespace detail

using detail::slice_t;

}  // namespace md_v2
}  // namespace ferrugo
