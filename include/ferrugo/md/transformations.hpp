#pragma once

#include <ferrugo/md/array.hpp>

namespace ferrugo
{
namespace md
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

struct subslice_fn
{
    struct impl
    {
        std::size_t m_dim;

        template <class T, std::size_t D, std::enable_if_t<(D > 1), int> = 0>
        auto operator()(const array_ref<T, D>& a) const -> array_ref<T, D - 1>
        {
            return array_ref<T, D - 1>{ a.get(typename array_ref<T, D>::location_type{}), shape(a).erase(m_dim) };
        }
    };

    constexpr auto operator()(std::size_t dim) const -> impl
    {
        return { dim };
    }
};

}  // namespace detail

static constexpr inline auto swap_axes = detail::swap_axes_fn{};
static constexpr inline auto subslice = detail::subslice_fn{};

}  // namespace md
}  // namespace ferrugo
