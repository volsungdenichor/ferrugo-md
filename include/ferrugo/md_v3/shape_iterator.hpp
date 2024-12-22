#pragma once

#include <ferrugo/core/iterator_interface.hpp>
#include <ferrugo/core/subrange.hpp>
#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/types.hpp>

namespace ferrugo
{
namespace md_v3
{

namespace detail
{

template <std::size_t D>
struct shape_iter
{
    dim_t<D> m_shape;
    location_t<D> m_loc;

    shape_iter() = default;

    shape_iter(dim_t<D> shape, location_t<D> loc) : m_shape{ std::move(shape) }, m_loc{ std::move(loc) }
    {
    }

    auto deref() const -> location_t<D>
    {
        return m_loc;
    }

    auto is_equal(const shape_iter& other) const -> bool
    {
        return m_loc == other.m_loc;
    }

    void inc()
    {
        inc(0);
    }

    void inc(std::size_t d)
    {
        ++m_loc[d];
        if (m_loc[d] == m_shape[d].size)
        {
            if (d + 1 < D)
            {
                m_loc[d] = 0;
                inc(d + 1);
            }
        }
    }
};

template <std::size_t D>
using shape_iterator = core::iterator_interface<shape_iter<D>>;

struct iterate_shape_fn
{
    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> core::subrange<shape_iterator<D>>
    {
        const auto begin = shape_iterator<D>{ item, location_t<D>{} };

        location_t<D> last{};
        last[D - 1] = item[D - 1].size;
        const auto end = shape_iterator<D>{ item, last };

        return { begin, end };
    }
};

}  // namespace detail

using detail::shape_iterator;
static constexpr inline auto iter = detail::iterate_shape_fn{};

}  // namespace md_v3
}  // namespace ferrugo
