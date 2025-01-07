#pragma once

#include <ferrugo/md_v3/lut.hpp>
#include <ferrugo/md_v3/types.hpp>
#include <numeric>

namespace ferrugo
{
namespace md_v3
{

using histogram_t = std::array<std::size_t, 256>;

template <class Range>
auto make_histogram(Range&& range) -> histogram_t
{
    histogram_t result = {};
    std::fill(std::begin(result), std::end(result), 0);

    for (byte v : range)
    {
        ++result[v];
    }

    return result;
}

inline auto accumulate_histogram(const histogram_t& histogram) -> histogram_t
{
    histogram_t result = {};
    std::partial_sum(std::begin(histogram), std::end(histogram), std::begin(result));
    return result;
}

inline auto equalize(const histogram_t& histogram) -> transform_fn
{
    const auto cum_hist = accumulate_histogram(histogram);
    const auto size = cum_hist.back();

    const auto min_level = std::invoke(
        [=]() -> std::size_t
        {
            std::size_t i = 0;
            while (i < 256 && cum_hist[i] == 0)
            {
                ++i;
            }
            return i;
        });

    return [=](int v) { return int(255 * (cum_hist[v] - min_level) / (size - min_level)); };
}

inline auto stretch(const histogram_t& histogram) -> transform_fn
{
    const auto min = std::invoke(
        [&]() -> int
        {
            int res = 0;
            while (histogram[res] == 0 && res < 255)
            {
                ++res;
            }
            return res;
        });

    const auto max = std::invoke(
        [&]() -> int
        {
            int res = 255;
            while (histogram[res] == 0 && res > 0)
            {
                --res;
            }
            return res;
        });

    return levels_adjustment({ min, max }, { 0, 255 });
}

}  // namespace md_v3
}  // namespace ferrugo
