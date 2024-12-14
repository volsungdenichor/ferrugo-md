#pragma once

#include <ferrugo/md/shape.hpp>

namespace ferrugo
{
namespace md
{

using byte = unsigned char;

template <class T, std::size_t D>
class array_ref
{
public:
    using value_type = std::remove_const_t<T>;
    using shape_type = shape_t<D>;
    using shape_iterator = typename shape_type::iterator;
    using location_type = location_t<D>;
    using reference = T&;
    using pointer = T*;

    array_ref(pointer ptr, shape_type shape) : m_ptr{ (byte*)ptr }, m_shape{ std::move(shape) }
    {
    }

    template <class T_ = T, class = std::enable_if_t<!std::is_const_v<T_>>>
    auto operator=(T_ value) -> array_ref&
    {
        std::fill(begin(), end(), value);
        return *this;
    }

    auto operator=(std::initializer_list<value_type> values) -> array_ref&
    {
        std::copy(std::begin(values), std::end(values), begin());
        return *this;
    }

    template <std::size_t Dim>
    auto sub(const location_type& this_loc, dim_array_t<Dim> new_shape) const -> array_ref<T, Dim>
    {
        T* new_ptr = get(this_loc);
        return { new_ptr, shape_t<Dim>{ std::move(new_shape) } };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto slice(std::size_t dim, index_t n) const -> array_ref<T, D - 1>
    {
        location_type loc = {};
        loc[dim] = n;
        dim_array_t<D_ - 1> new_shape;
        for (std::size_t i = 0; i + 1 < D_; ++i)
        {
            new_shape[i] = m_shape.dim(i < dim ? i : i + 1);
        }
        return sub(loc, new_shape);
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto operator[](index_t n) const -> array_ref<T, D_ - 1>
    {
        return slice(0, n);
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ == 1), int> = 0>
    auto operator[](index_t n) const -> reference
    {
        return (*this)[location_t<1>{ n }];
    }

    auto operator[](const location_type& loc) const -> reference
    {
        return *get(loc);
    }

    template <std::size_t D_ = D, class = std::enable_if_t<(D_ > 0)>>
    auto operator[](const location_t<D_ - 1>& loc) const -> array_ref<T, 1>
    {
        location_type this_loc = {};
        std::copy(std::begin(loc), std::end(loc), std::begin(this_loc));
        dim_array_t<1> new_shape{ m_shape.dim(D - 1) };
        return sub(this_loc, new_shape);
    }

    auto get(const location_type& loc) const -> pointer
    {
        return reinterpret_cast<T*>(m_ptr + offset(m_shape, loc));
    }

    friend std::ostream& operator<<(std::ostream& os, const array_ref& item)
    {
        os << item.m_shape;
        for (T& v : item)
        {
            os << " " << (int)v;
        }
        return os;
    }

    auto shape() const -> const shape_type&
    {
        return m_shape;
    }

    struct iter
    {
        const array_ref* m_self;
        shape_iterator m_iter;

        iter() = default;

        iter(const array_ref& self, shape_iterator iter) : m_self{ &self }, m_iter{ std::move(iter) }
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

        auto is_equal(const iter& other) const -> bool
        {
            return m_iter == other.m_iter;
        }
    };

    using iterator = ferrugo::core::iterator_interface<iter>;

    auto begin() const -> iterator
    {
        return iterator{ *this, std::begin(m_shape) };
    }

    auto end() const -> iterator
    {
        return iterator{ *this, std::end(m_shape) };
    }

private:
    byte* m_ptr;
    shape_type m_shape;
};

struct slices_fn
{
    template <class T, std::size_t D>
    struct iter
    {
        const array_ref<T, D>* m_self;
        std::size_t m_dim;
        std::ptrdiff_t m_n;

        iter() : m_self{}, m_dim{}, m_n{}
        {
        }

        iter(const array_ref<T, D>& self, std::size_t dim, std::ptrdiff_t n) : m_self{ &self }, m_dim{ dim }, m_n{ n }
        {
        }

        auto deref() const -> array_ref<T, D - 1>
        {
            return m_self->slice(m_dim, m_n);
        }

        void advance(std::ptrdiff_t offset)
        {
            m_n += offset;
        }

        auto distance_to(const iter& other) const -> std::ptrdiff_t
        {
            return other.m_n - m_n;
        }
    };

    template <class T, std::size_t D>
    using iterator = ferrugo::core::iterator_interface<iter<T, D>>;

    struct impl
    {
        std::size_t m_d;

        template <class T, std::size_t D>
        auto operator()(const array_ref<T, D>& a) const -> ferrugo::core::subrange<iterator<T, D>>
        {
            return {
                iterator<T, D>{ a, m_d, std::ptrdiff_t(0) },
                iterator<T, D>{ a, m_d, a.shape().dim(m_d).size },
            };
        }
    };

    auto operator()(std::size_t d) const -> impl
    {
        return { d };
    }
};

struct subslices_fn
{
    template <class T, std::size_t D>
    struct iter
    {
        const array_ref<T, D>* m_self;
        typename shape_t<D - 1>::iterator m_inner;

        iter() : m_self{}, m_inner()
        {
        }

        iter(const array_ref<T, D>& self, typename shape_t<D - 1>::iterator inner) : m_self{ &self }, m_inner{ inner }
        {
        }

        auto deref() const -> array_ref<T, 1>
        {
            return (*m_self)[*m_inner];
        }

        void inc()
        {
            ++m_inner;
        }

        auto is_equal(const iter& other) const -> bool
        {
            return m_inner == other.m_inner;
        }
    };

    template <class T, std::size_t D>
    using iterator = ferrugo::core::iterator_interface<iter<T, D>>;

    struct impl
    {
        std::size_t m_d;
        index_t m_n;

        template <class T, std::size_t D>
        auto operator()(const array_ref<T, D>& a) const -> ferrugo::core::subrange<iterator<T, D>>
        {
            const auto slice = a.slice(m_d, m_n).shape();
            return {
                iterator<T, D>{ a, slice.begin() },
                iterator<T, D>{ a, slice.end() },
            };
        }
    };

    auto operator()(std::size_t d, index_t n) const -> impl
    {
        return { d, n };
    }
};

static constexpr inline auto slices = slices_fn{};
static constexpr inline auto subslices = subslices_fn{};

}  // namespace md
}  // namespace ferrugo
