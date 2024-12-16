#pragma once

#include <ferrugo/md/shape.hpp>

namespace ferrugo
{
namespace md
{

using byte = unsigned char;

struct slice_t
{
    std::optional<index_t> start = std::nullopt;
    std::optional<index_t> stop = std::nullopt;
    std::optional<index_t> step = std::nullopt;
};

inline auto at(index_t v) -> slice_t
{
    return v != -1 ? slice_t{ v, v + 1, {} } : slice_t{ -1, {} };
}

inline auto indices(const slice_t& s, index_t n) -> std::array<index_t, 3>
{
    const auto step = s.step.value_or(1);
    const auto clamp = [&](index_t v) { return std::max(index_t(0), std::min(v, n)); };
    const auto adjust_val = [&](index_t v) { return clamp(v >= 0 ? v : n + v); };
    if (step > 0)
    {
        const auto start = s.start ? adjust_val(*s.start) : 0;
        const auto stop = s.stop ? adjust_val(*s.stop) : n;
        return { start, stop, step };
    }
    if (step < 0)
    {
        const auto start = s.start ? adjust_val(*s.start) : n - 1;
        const auto stop = s.stop ? adjust_val(*s.stop) : -1;
        return { start, stop, step };
    }
    throw std::runtime_error{ "step cannot be zero" };
}

template <std::size_t D>
using slice_array_t = std::array<slice_t, D>;

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

    auto as_const() const -> array_ref<std::add_const_t<T>, D>
    {
        return { get(), m_shape };
    }

    operator array_ref<std::add_const_t<T>, D>() const
    {
        return as_const();
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
        return array_ref<T, Dim>{ get(this_loc), shape_t<Dim>{ std::move(new_shape) } };
    }

    auto operator[](const slice_array_t<D>& slices) const -> array_ref
    {
        static const auto handle = [](const dim_t& d, const slice_t& s) -> std::pair<index_t, dim_t>
        {
            const auto [start, stop, step] = indices(s, d.size);
            const auto size = std::invoke(
                [&]() -> index_t
                {
                    index_t res = 0;
                    if (step > 0)
                    {
                        for (index_t val = start; val < stop; val += step, ++res)
                        {
                        }
                    }
                    else
                    {
                        for (index_t val = start; val > stop; val += step, ++res)
                        {
                        }
                    }
                    return res;
                });
            return { start, dim_t{ std::max<index_t>(0, size), d.stride * step } };
        };

        location_type start;
        dim_array_t<D> new_shape;

        for (std::size_t d = 0; d < D; ++d)
        {
            std::tie(start[d], new_shape[d]) = handle(shape().dim(d), slices[d]);
        }

        return array_ref{ get(start), shape_type{ new_shape } };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto slice(std::size_t dim, index_t n) const -> array_ref<T, D - 1>
    {
        const location_type loc = std::invoke(
            [&]() -> location_type
            {
                location_type res = {};
                std::fill(std::begin(res), std::end(res), 0);
                res[dim] = n;
                return res;
            });

        const dim_array_t<D_ - 1> new_shape = std::invoke(
            [&]() -> dim_array_t<D_ - 1>
            {
                dim_array_t<D_ - 1> res;
                for (std::size_t i = 0; i + 1 < D_; ++i)
                {
                    res[i] = m_shape.dim(i < dim ? i : i + 1);
                }
                return res;
            });

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

    auto get() const -> pointer
    {
        return (pointer)m_ptr;
    }

    auto get(const location_type& loc) const -> pointer
    {
        return reinterpret_cast<pointer>(m_ptr + offset(m_shape, loc));
    }

    auto operator[](const location_type& loc) const -> reference
    {
        return *get(loc);
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

struct swap_axes_fn
{
    struct impl
    {
        std::size_t m_a;
        std::size_t m_b;

        template <class T, std::size_t D>
        auto operator()(const array_ref<T, D>& a) const -> array_ref<T, D>
        {
            auto dims = a.shape().dims();
            std::swap(dims[m_a], dims[m_b]);
            return array_ref<T, D>{ a.get(), shape_t<D>{ dims } };
        }
    };

    constexpr auto operator()(std::size_t a, std::size_t b) const -> impl
    {
        return { a, b };
    }
};

static constexpr inline auto slices = slices_fn{};
static constexpr inline auto subslices = subslices_fn{};
static constexpr inline auto swap_axes = swap_axes_fn{};
static constexpr inline auto transpose = swap_axes(0, 1);

}  // namespace md
}  // namespace ferrugo
