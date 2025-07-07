#pragma once

#include <ferrugo/md/location_iterator.hpp>

namespace ferrugo
{
namespace md
{

namespace detail
{

template <class T, std::size_t D>
struct element_iter
{
    byte* m_ptr;
    location_iterator<D> m_inner;

    element_iter() = default;

    element_iter(byte* ptr, location_iterator<D> inner) : m_ptr{ ptr }, m_inner{ std::move(inner) }
    {
    }

    auto deref() const -> T&
    {
        return *(T*)(m_ptr + flat_offset(m_inner.m_impl.m_shape, m_inner.m_impl.m_loc));
    }

    auto is_equal(const element_iter& other) const -> bool
    {
        return m_inner == other.m_inner;
    }

    void inc()
    {
        ++m_inner;
    }
};

template <class T, std::size_t D>
using element_iterator = core::iterator_interface<element_iter<T, D>>;

}  // namespace detail

}  // namespace md
}  // namespace ferrugo
