#pragma once

#include <cstdint>
#include <ferrugo/md_v2/access.hpp>
#include <ferrugo/md_v2/shape_iterator.hpp>
#include <ferrugo/md_v2/slice.hpp>
#include <ferrugo/md_v2/types.hpp>
#include <functional>

namespace ferrugo
{
namespace md_v2
{

using byte = std::uint8_t;

template <class T, std::size_t D>
class array_ref
{
public:
    using value_type = std::remove_const_t<T>;
    using shape_type = dim_t<D>;
    using location_type = location_t<D>;
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
        return (pointer)(m_ptr + offset(m_shape, loc));
    }

    auto operator[](const location_type& loc) const -> reference
    {
        return *get(loc);
    }

    auto slice(const slice_t<D>& slices) const -> array_ref
    {
        dim_t<D> new_shape;

        for (std::size_t d = 0; d < D; ++d)
        {
            at(new_shape, d) = handle(m_shape, at(slices, d));
        }
        std::cout << "shape " << m_shape << " -> new " << new_shape << "\n";
        return array_ref{ get(), shape_type{ new_shape } };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto subslice(std::size_t dim, std::ptrdiff_t n) const -> array_ref<T, D_ - 1>
    {
        const location_type loc = std::invoke(
            [&]() -> location_type
            {
                location_type res = {};
                std::fill(std::begin(res), std::end(res), 0);
                res[dim] = n;
                return res;
            });

        const dim_t<D_ - 1> new_shape = std::invoke(
            [&]() -> dim_t<D_ - 1>
            {
                dim_t<D_ - 1> res;
                for (std::size_t i = 0; i + 1 < D_; ++i)
                {
                    res[i] = m_shape[i < dim ? i : i + 1];
                }
                return res;
            });

        return array_ref<T, D_ - 1>{ get(loc), new_shape };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto operator[](std::ptrdiff_t n) const -> array_ref<T, D_ - 1>
    {
        return subslice(std::size_t{ 0 }, n);
    }

    struct iter_type
    {
        const array_ref* m_self;
        shape_iterator<D> m_iter;

        iter_type() = default;

        iter_type(const array_ref& self, shape_iterator<D> iter) : m_self{ &self }, m_iter{ std::move(iter) }
        {
        }

        auto deref() const -> T&
        {
            return *m_self->get(*m_iter);
        }

        void inc()
        {
            ++m_iter;
        }

        auto is_equal(const iter_type& other) const -> bool
        {
            return m_iter == other.m_iter;
        }
    };

    using iterator = ferrugo::core::iterator_interface<iter_type>;

    auto begin() const -> iterator
    {
        return iterator{ *this, iter(m_shape).begin() };
    }

    auto end() const -> iterator
    {
        return iterator{ *this, iter(m_shape).end() };
    }

private:
    byte* m_ptr;
    shape_type m_shape;
};

}  // namespace md_v2
}  // namespace ferrugo
