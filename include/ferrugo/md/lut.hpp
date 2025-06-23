#pragma once

#include <cmath>
#include <ferrugo/md/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md
{

struct transform_fn : public std::function<int(int)>
{
    using base_t = std::function<int(int)>;
    using base_t::base_t;
};

inline auto operator*(const transform_fn& lhs, const transform_fn& rhs) -> transform_fn
{
    return [=](int v) { return rhs(lhs(v)); };
}

struct lut_t
{
    std::array<byte, 256> m_values;

    explicit lut_t(const transform_fn& transform)
    {
        for (std::size_t i = 0; i < 256; ++i)
        {
            m_values[i] = to_byte(transform(i));
        }
    }

    static auto to_byte(int v) -> byte
    {
        return static_cast<byte>(std::clamp(v, 0, 255));
    }

    auto operator()(byte value) const -> byte
    {
        return m_values[value];
    }
};

inline auto identity() -> transform_fn
{
    return [](int v) { return v; };
}

inline auto brightness(int value) -> transform_fn
{
    return [=](int v) { return v + value; };
}

inline auto contrast(float value, int center = 128) -> transform_fn
{
    return [=](int v) { return int(value * (v - center) + center); };
}

inline auto threshold(int value, int lower, int upper) -> transform_fn
{
    return [=](int v) { return v < value ? lower : upper; };
}

inline auto threshold(int value) -> transform_fn
{
    return threshold(value, 0, 255);
}

inline auto negative() -> transform_fn
{
    return [](int v) { return 255 - v; };
}

inline auto exposition(float value) -> transform_fn
{
    if (value < 0)
    {
        return identity();
    }
    return [=](int v) { return int(v * value); };
}

inline auto levels_adjustment(const std::tuple<int, int>& in, const std::tuple<int, int>& out, float gamma = 1.F)
    -> transform_fn
{
    static const auto lower = [](const std::tuple<int, int>& t) -> int { return std::get<0>(t); };
    static const auto upper = [](const std::tuple<int, int>& t) -> int { return std::get<1>(t); };
    static const auto size = [](const std::tuple<int, int>& t) -> int { return upper(t) - lower(t); };

    return [=](int v) -> int
    {
        if (v < lower(in))
        {
            return lower(out);
        }

        if (v > upper(in))
        {
            return upper(out);
        }

        return int(lower(out) + size(out) * std::pow(float(v - lower(in)) / size(in), 1.F / gamma));
    };
}

inline auto gamma(float value) -> transform_fn
{
    return levels_adjustment({ 0, 255 }, { 0, 255 }, value);
}

}  // namespace md
}  // namespace ferrugo
