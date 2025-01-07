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

namespace detail
{

struct equalize_fn
{
    auto operator()(const histogram_t& histogram) const -> transform_fn
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
};

struct stretch_fn
{
    auto operator()(const histogram_t& histogram) const -> transform_fn
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
};

struct otsu_fn
{
    auto operator()(const histogram_t& histogram) const -> transform_fn
    {
        const auto t = std::invoke(
            [&]() -> int
            {
                const auto cum_hist = accumulate_histogram(histogram);

                const auto total = cum_hist.back();

                std::size_t sum = 0;

                for (std::size_t i = 0; i < 256; ++i)
                {
                    sum += i * histogram[i];
                }

                std::size_t sumb = 0;

                std::array<std::size_t, 2> threshold{ 0, 0 };

                float max = 0.F;

                for (std::size_t i = 0; i < 256; ++i)
                {
                    const auto wb = cum_hist[i];

                    if (wb == 0)
                    {
                        continue;
                    }

                    const auto wf = total - wb;

                    if (wf == 0)
                    {
                        continue;
                    }

                    sumb += i * histogram[i];

                    const auto mb = (float)sumb / wb;
                    const auto mf = (float)(sum - sumb) / wf;

                    const auto between = wb * wf * (mb - mf) * (mb - mf);

                    if (between >= max)
                    {
                        threshold[0] = i;

                        if (between > max)
                        {
                            threshold[1] = i;
                        }

                        max = between;
                    }
                }

                return int(0.5 * threshold[0] + 0.5 * threshold[1]);
            });

        return threshold(t);
    }
};

}  // namespace detail

static constexpr inline auto stretch = detail::stretch_fn{};
static constexpr inline auto equalize = detail::equalize_fn{};
static constexpr inline auto otsu = detail::otsu_fn{};

}  // namespace md_v3
}  // namespace ferrugo
