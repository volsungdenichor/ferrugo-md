#pragma once

#include <cstdint>
#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/shape_iterator.hpp>
#include <ferrugo/md_v3/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md_v3
{

using byte = std::uint8_t;

template <class T, std::size_t D>
class array_ref
{
public:
    using value_type = std::remove_const_t<T>;
    using shape_type = dim_t<D>;
    using location_type = location_t<D>;
    using slice_type = slice_t<D>;
    using reference = T&;
    using pointer = T*;

    array_ref(pointer ptr, shape_type shape) : m_ptr{ (byte*)ptr }, m_shape{ std::move(shape) }
    {
    }

    auto as_const() const -> array_ref<std::add_const_t<T>, D>
    {
        return { get(), m_shape };
    }

    operator array_ref<std::add_const_t<T>, D>() const
    {
        return as_const();
    }

    const shape_type& shape() const
    {
        return m_shape;
    }

    auto get() const -> pointer
    {
        return (pointer)m_ptr;
    }

    auto get(const location_type& loc) const -> pointer
    {
        byte* ptr = (m_ptr + offset(m_shape, loc));
        return (pointer)ptr;
    }

    auto operator[](const location_type& loc) const -> reference
    {
        return *get(loc);
    }

    auto slice(const slice_type& slices) const -> array_ref
    {
        return array_ref{ get(), apply_slice(m_shape, slices) };
    }

private:
    byte* m_ptr;
    shape_type m_shape;
};

}  // namespace md_v3
}  // namespace ferrugo
