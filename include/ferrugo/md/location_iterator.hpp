#pragma once

#include <ferrugo/core/iterator_interface.hpp>
#include <ferrugo/core/subrange.hpp>
#include <ferrugo/md/access.hpp>
#include <ferrugo/md/types.hpp>

namespace ferrugo
{
namespace md
{

namespace detail
{

template <std::size_t D>
struct location_iter
{
    dim_t<D> m_shape;
    location_t<D> m_loc;

    location_iter() = default;

    location_iter(dim_t<D> shape, location_t<D> loc) : m_shape{ std::move(shape) }, m_loc{ std::move(loc) }
    {
    }

    auto deref() const -> location_t<D>
    {
        return m_loc;
    }

    auto is_equal(const location_iter& other) const -> bool
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
using location_iterator = core::iterator_interface<location_iter<D>>;

struct iterate_locations_fn
{
    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> core::subrange<location_iterator<D>>
    {
        const auto begin = location_iterator<D>{ item, location_t<D>{} };

        location_t<D> last{};
        last[D - 1] = item[D - 1].size;
        const auto end = location_iterator<D>{ item, last };

        return { begin, end };
    }
};

}  // namespace detail

using detail::location_iterator;
static constexpr inline auto locations = adaptable<detail::iterate_locations_fn>{};

}  // namespace md
}  // namespace ferrugo
