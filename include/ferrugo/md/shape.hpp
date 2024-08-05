#pragma once

#include <array>
#include <cstddef>
#include <ferrugo/core/iterator_interface.hpp>
#include <ferrugo/core/subrange.hpp>
#include <iostream>

namespace ferrugo
{
namespace md
{

template <std::size_t D>
struct location_t : std::array<std::ptrdiff_t, D>
{
    using base_t = std::array<std::ptrdiff_t, D>;

    using base_t::base_t;

    location_t()
    {
        std::fill(base_t::begin(), base_t::end(), 0);
    }

    template <class... Tail>
    location_t(std::ptrdiff_t head, Tail... tail) : base_t{ { head, tail... } }
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const location_t& item)
    {
        os << "[";
        for (std::size_t d = 0; d < D; ++d)
        {
            if (d != 0)
            {
                os << " ";
            }
            os << item[d];
        }
        os << "]";
        return os;
    }
};

struct dim_t : std::array<std::ptrdiff_t, 2>
{
    using base_t = std::array<std::ptrdiff_t, 2>;

    dim_t() = default;

    dim_t(std::ptrdiff_t size, std::ptrdiff_t stride) : base_t{ { size, stride } }
    {
    }
};

template <std::size_t D>
struct shape_t
{
    using dim_array = std::array<dim_t, D>;
    dim_array m_dims;

    shape_t() = default;

    shape_t(dim_array dims) : m_dims(std::move(dims))
    {
    }

    template <class... Args>
    shape_t(Args&&... args) : m_dims{ { dim_t{ std::forward<Args>(args) }... } }
    {
    }

    const dim_t& dim(std::size_t d) const
    {
        return m_dims[d];
    }

    struct iter
    {
        dim_array m_shape;
        location_t<D> m_loc;

        iter() = default;

        iter(const shape_t& shape, location_t<D> loc) : m_shape{ shape.m_dims }, m_loc(std::move(loc))
        {
        }

        location_t<D> deref() const
        {
            return m_loc;
        }

        bool is_equal(const iter& other) const
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
            if (m_loc[d] == m_shape[d][0])
            {
                if (d + 1 < D)
                {
                    m_loc[d] = 0;
                    inc(d + 1);
                }
            }
        }
    };

    using iterator = ferrugo::core::iterator_interface<iter>;

    auto begin() const -> iterator
    {
        return iterator{ *this, location_t<D>{} };
    }

    auto end() const -> iterator
    {
        location_t<D> loc{};
        loc[D - 1] = m_dims[D - 1][0];
        return iterator{ *this, loc };
    }
};

namespace detail
{

template <class Impl>
struct shape_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        static const auto impl = Impl{};
        return impl(item);
    }

    template <std::size_t D>
    auto operator()(const shape_t<D>& item) const -> location_t<D>
    {
        location_t<D> result;
        for (std::size_t d = 0; d < D; ++d)
        {
            result[d] = (*this)(item.dim(d));
        }
        return result;
    }
};

struct min_fn
{
    auto operator()(const dim_t&) const -> std::ptrdiff_t
    {
        return 0;
    }
};

struct max_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        return item[0] - 1;
    }
};

struct lower_fn
{
    auto operator()(const dim_t&) const -> std::ptrdiff_t
    {
        return 0;
    }
};

struct upper_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        return item[0];
    }
};

struct size_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        return item[0];
    }
};

struct stride_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        return item[1];
    }
};

struct offset_fn
{
    auto operator()(const dim_t& item, std::ptrdiff_t loc) const -> std::ptrdiff_t
    {
        return loc * item[1];
    }

    template <std::size_t D>
    auto operator()(const shape_t<D>& item, const location_t<D>& loc) const -> std::ptrdiff_t
    {
        std::ptrdiff_t result = 0;
        for (std::size_t d = 0; d < D; ++d)
        {
            result += (*this)(item.dim(d), loc[d]);
        }
        return result;
    }
};

struct volume_fn
{
    auto operator()(const dim_t& item) const -> std::ptrdiff_t
    {
        return item[0];
    }

    template <std::size_t D>
    auto operator()(const shape_t<D>& item) const -> std::ptrdiff_t
    {
        std::ptrdiff_t result = 1;
        for (std::size_t d = 0; d < D; ++d)
        {
            result *= (*this)(item.dim(d));
        }
        return result;
    }
};

}  // namespace detail

static constexpr inline auto min = detail::shape_fn<detail::min_fn>{};
static constexpr inline auto max = detail::shape_fn<detail::max_fn>{};
static constexpr inline auto lower = detail::shape_fn<detail::lower_fn>{};
static constexpr inline auto upper = detail::shape_fn<detail::upper_fn>{};
static constexpr inline auto size = detail::shape_fn<detail::size_fn>{};
static constexpr inline auto stride = detail::shape_fn<detail::stride_fn>{};
static constexpr inline auto offset = detail::offset_fn{};
static constexpr inline auto volume = detail::volume_fn{};

template <std::size_t D>
std::ostream& operator<<(std::ostream& os, const shape_t<D>& item)
{
    return os << "size=" << size(item) << " stride=" << stride(item);
}

}  // namespace md
}  // namespace ferrugo
