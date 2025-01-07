#pragma once

#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/ref_iterator.hpp>
#include <ferrugo/md_v3/shape_iterator.hpp>
#include <ferrugo/md_v3/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md_v3
{

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
    using iterator = detail::ref_iterator<T, D>;

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

    auto begin() const -> iterator
    {
        return iterator{ m_ptr, m_shape, location_t<D>{} };
    }

    auto end() const -> iterator
    {
        location_t<D> last = {};
        last[D - 1] = m_shape[D - 1].size;
        return iterator{ m_ptr, m_shape, last };
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

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto subslice(std::size_t dim, location_base_t n) const -> array_ref<T, D - 1>
    {
        const location_base_t actual_n = n > 0 ? n : m_shape[dim].size + n;
        if (!(0 <= actual_n && actual_n < m_shape[dim].size))
        {
            throw std::out_of_range("out of range");
        }
        const auto loc = std::invoke(
            [&]() -> location_type
            {
                location_type res = {};
                std::fill(std::begin(res), std::end(res), 0);
                res[dim] = actual_n;
                return res;
            });

        return array_ref<T, D - 1>{ get(loc), m_shape.erase(dim) };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto operator[](location_base_t n) const -> array_ref<T, D_ - 1>
    {
        return subslice(0, n);
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ == 1), int> = 0>
    auto operator[](location_base_t n) const -> reference
    {
        return (*this)[location_t<1>{ n }];
    }

private:
    byte* m_ptr;
    shape_type m_shape;
};

}  // namespace md_v3
}  // namespace ferrugo
