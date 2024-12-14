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

using index_t = std::ptrdiff_t;
using flat_offset_t = std::ptrdiff_t;
using volume_t = std::ptrdiff_t;

template <std::size_t D>
struct location_t : std::array<index_t, D>
{
    using base_t = std::array<index_t, D>;

    using base_t::base_t;

    location_t()
    {
        std::fill(base_t::begin(), base_t::end(), 0);
    }

    template <class... Tail>
    location_t(index_t head, Tail... tail) : base_t{ { head, tail... } }
    {
        static_assert(sizeof...(tail) + 1 == D, "all values required to be set");
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

struct dim_t
{
    index_t size;
    index_t stride;
};

template <std::size_t D>
using dim_array_t = std::array<dim_t, D>;

template <std::size_t D>
class shape_t
{
public:
    using location_type = location_t<D>;
    using dim_array_type = dim_array_t<D>;

    shape_t() = default;

    shape_t(dim_array_type dims) : m_dims(std::move(dims))
    {
    }

    shape_t(const shape_t&) = default;
    shape_t(shape_t&&) = default;

    template <class... Tail>
    shape_t(dim_t head, Tail&&... tail) : m_dims{ { head, dim_t{ std::forward<Tail>(tail) }... } }
    {
        static_assert(sizeof...(tail) + 1 == D, "all dimensions required");
    }

    auto dim(std::size_t d) const -> const dim_t&
    {
        return m_dims[d];
    }

    struct iter
    {
        dim_array_type m_shape;
        location_type m_loc;

        iter() = default;

        iter(const shape_t& shape, location_t<D> loc) : m_shape{ shape.m_dims }, m_loc(std::move(loc))
        {
        }

        auto deref() const -> location_type
        {
            return m_loc;
        }

        auto is_equal(const iter& other) const -> bool
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

    using iterator = ferrugo::core::iterator_interface<iter>;

    auto begin() const -> iterator
    {
        return iterator{ *this, location_type{} };
    }

    auto end() const -> iterator
    {
        location_type loc{};
        loc.back() = m_dims[D - 1].size;
        return iterator{ *this, loc };
    }

private:
    dim_array_type m_dims;
};

namespace detail
{

template <class Impl>
struct shape_getter_fn
{
    auto operator()(const dim_t& item) const -> index_t
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
    auto operator()(const dim_t&) const -> index_t
    {
        return 0;
    }
};

struct max_fn
{
    auto operator()(const dim_t& item) const -> index_t
    {
        return item.size - 1;
    }
};

struct lower_fn
{
    auto operator()(const dim_t&) const -> index_t
    {
        return 0;
    }
};

struct upper_fn
{
    auto operator()(const dim_t& item) const -> index_t
    {
        return item.size;
    }
};

struct size_fn
{
    auto operator()(const dim_t& item) const -> index_t
    {
        return item.size;
    }
};

struct stride_fn
{
    auto operator()(const dim_t& item) const -> index_t
    {
        return item.stride;
    }
};

struct offset_fn
{
    auto operator()(const dim_t& item, index_t loc) const -> flat_offset_t
    {
        return loc * item.stride;
    }

    template <std::size_t D>
    auto operator()(const shape_t<D>& item, const location_t<D>& loc) const -> flat_offset_t
    {
        flat_offset_t result = 0;
        for (std::size_t d = 0; d < D; ++d)
        {
            result += (*this)(item.dim(d), loc[d]);
        }
        return result;
    }
};

struct volume_fn
{
    auto operator()(const dim_t& item) const -> volume_t
    {
        return item.size;
    }

    template <std::size_t D>
    auto operator()(const shape_t<D>& item) const -> volume_t
    {
        volume_t result = 1;
        for (std::size_t d = 0; d < D; ++d)
        {
            result *= (*this)(item.dim(d));
        }
        return result;
    }
};

}  // namespace detail

static constexpr inline auto min = detail::shape_getter_fn<detail::min_fn>{};
static constexpr inline auto max = detail::shape_getter_fn<detail::max_fn>{};
static constexpr inline auto lower = detail::shape_getter_fn<detail::lower_fn>{};
static constexpr inline auto upper = detail::shape_getter_fn<detail::upper_fn>{};
static constexpr inline auto size = detail::shape_getter_fn<detail::size_fn>{};
static constexpr inline auto stride = detail::shape_getter_fn<detail::stride_fn>{};
static constexpr inline auto offset = detail::offset_fn{};
static constexpr inline auto volume = detail::volume_fn{};

template <std::size_t D>
std::ostream& operator<<(std::ostream& os, const shape_t<D>& item)
{
    return os << "size=" << size(item) << " stride=" << stride(item);
}

}  // namespace md
}  // namespace ferrugo
