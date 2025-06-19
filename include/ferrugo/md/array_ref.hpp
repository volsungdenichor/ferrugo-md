#pragma once

#include <ferrugo/md/access.hpp>
#include <ferrugo/md/element_iterator.hpp>
#include <ferrugo/md/location_iterator.hpp>
#include <ferrugo/md/types.hpp>
#include <functional>
#include <sstream>

namespace ferrugo
{
namespace md
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
    using iterator = detail::element_iterator<T, D>;

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

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto sub_1d(std::size_t dim, const location_t<D - 1>& loc) const -> array_ref<T, 1>
    {
        const auto new_loc = std::invoke(
            [&]() -> location_type
            {
                location_type res = {};
                auto iter = std::copy(loc.begin(), loc.begin() + dim, res.begin());
                *iter++ = 0;
                std::copy(loc.begin() + dim, loc.end(), iter);
                return res;
            });

        return array_ref<T, 1>{ get(new_loc), dim_t<1>{ m_shape[dim] } };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto sub(std::size_t dim, location_base_t n) const -> array_ref<T, D - 1>
    {
        const location_base_t actual_n = n >= 0 ? n : m_shape[dim].size + n;
        if (!(0 <= actual_n && actual_n < m_shape[dim].size))
        {
            throw std::out_of_range{ str("out of range: ", "n=", actual_n, " size=", m_shape[dim].size) };
        }
        const auto new_loc = std::invoke(
            [&]() -> location_type
            {
                location_type res = {};
                std::fill(std::begin(res), std::end(res), 0);
                res[dim] = actual_n;
                return res;
            });

        return array_ref<T, D - 1>{ get(new_loc), m_shape.erase(dim) };
    }

    auto slice(const slice_type& slices) const -> array_ref
    {
        return array_ref{ get(), apply_slice(m_shape, slices) };
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto operator[](const location_t<D - 1>& loc) const -> array_ref<T, 1>
    {
        return sub_1d(D_ - 1, loc);
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ > 1), int> = 0>
    auto operator[](location_base_t n) const -> array_ref<T, D_ - 1>
    {
        return sub(0, n);
    }

    template <std::size_t D_ = D, std::enable_if_t<(D_ == 1), int> = 0>
    auto operator[](location_base_t n) const -> reference
    {
        return (*this)[location_t<1>{ n }];
    }

    template <class T_ = T, std::enable_if_t<!std::is_const_v<T_>, int> = 0>
    auto operator=(T_ value) -> array_ref&
    {
        std::fill(begin(), end(), value);
        return *this;
    }

private:
    byte* m_ptr;
    shape_type m_shape;
};

struct make_array_ref_fn
{
    template <class T>
    static auto make_dim(const std::vector<T>& v) -> dim_t<1>
    {
        return dim_t<1>{ { static_cast<std::ptrdiff_t>(v.size()), sizeof(T) } };
    }

    template <class T>
    auto operator()(const std::vector<T>& v) const -> array_ref<const T, 1>
    {
        return array_ref<const T, 1>{ v.data(), make_dim(v) };
    }

    template <class T>
    auto operator()(std::vector<T>& v) const -> array_ref<T, 1>
    {
        return array_ref<T, 1>{ v.data(), make_dim(v) };
    }
};

static constexpr inline auto make_array_ref = make_array_ref_fn{};

}  // namespace md
}  // namespace ferrugo
