#pragma once

#include <ferrugo/md_v3/array_ref.hpp>

namespace ferrugo
{
namespace md_v3
{

namespace detail
{

struct swap_axes_fn
{
    struct impl
    {
        std::size_t m_a;
        std::size_t m_b;

        template <class T, std::size_t D>
        auto operator()(const array_ref<T, D>& a) const -> array_ref<T, D>
        {
            auto dims = a.shape();
            std::swap(dims[m_a], dims[m_b]);
            return array_ref<T, D>{ a.get(), dims };
        }
    };

    constexpr auto operator()(std::size_t a, std::size_t b) const -> impl
    {
        return { a, b };
    }
};

}  // namespace detail

static constexpr inline auto swap_axes = detail::swap_axes_fn{};

}  // namespace md_v3
}  // namespace ferrugo
