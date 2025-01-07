#pragma once

#include <cstdint>
#include <ferrugo/md_v3/shape_iterator.hpp>

namespace ferrugo
{
namespace md_v3
{

using byte = std::uint8_t;

namespace detail
{

template <class T, std::size_t D>
struct ref_iter
{
    byte* m_ptr;
    dim_t<D> m_shape;
    location_t<D> m_loc;

    ref_iter() = default;

    ref_iter(byte* ptr, dim_t<D> shape, location_t<D> loc)
        : m_ptr{ ptr }
        , m_shape{ std::move(shape) }
        , m_loc{ std::move(loc) }
    {
    }

    auto deref() const -> T&
    {
        byte* ptr = (m_ptr + offset(m_shape, m_loc));
        return *(T*)ptr;
    }

    auto is_equal(const ref_iter& other) const -> bool
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

template <class T, std::size_t D>
using ref_iterator = core::iterator_interface<ref_iter<T, D>>;

}  // namespace detail

}  // namespace md_v3
}  // namespace ferrugo
