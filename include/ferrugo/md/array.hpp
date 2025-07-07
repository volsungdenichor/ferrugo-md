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

namespace detail
{
inline auto apply_slice(const dim_base_t& dim, const slice_base_t& slice) -> std::tuple<dim_base_t, location_base_t>
{
    const auto step = slice.step.value_or(1);

    if (step == 0)
    {
        throw std::runtime_error{ "step cannot be zero" };
    }
    static const auto ensure_non_negative = [](location_base_t v) { return std::max(location_base_t(0), v); };
    const auto apply_size = [&](location_base_t v) { return v < 0 ? v + dim.size : v; };
    const auto clamp = [&](location_base_t v, location_base_t shift)
    { return std::max(location_base_t(shift), std::min(v, dim.size + shift)); };

    const auto [size, start]  //
        = step > 0            //
              ? std::invoke(
                  [&]() -> std::tuple<size_base_t, location_base_t>
                  {
                      const auto start = clamp(apply_size(slice.start.value_or(0)), 0);
                      const auto stop = clamp(apply_size(slice.stop.value_or(dim.size)), 0);
                      const auto size = ensure_non_negative(((stop - start) + step - 1) / step);
                      return std::tuple{ size, start };
                  })
              : std::invoke(
                  [&]() -> std::tuple<location_base_t, location_base_t>
                  {
                      const auto start = clamp(apply_size(slice.start.value_or(dim.size)), -1);
                      const auto stop = clamp(apply_size(slice.stop.value_or(0)), -1);
                      const auto size = ensure_non_negative(((stop - start) + step) / step);
                      return std::tuple{ size, start };
                  });
    return { dim_base_t{ std::min(size, dim.size), dim.stride * step }, start };
}

template <std::size_t D>
inline auto apply_slice(const dim_t<D>& dim, const slice_t<D>& slice) -> std::tuple<dim_t<D>, location_t<D>>
{
    std::tuple<dim_t<D>, location_t<D>> result;
    for (std::size_t d = 0; d < D; ++d)
    {
        std::tie(std::get<0>(result)[d], std::get<1>(result)[d]) = apply_slice(dim[d], slice[d]);
    }
    return result;
}

template <std::size_t D>
auto create_shape(const size_t<D>& size, location_base_t stride) -> dim_t<D>
{
    dim_t<D> result = {};
    for (int d = D - 1; d >= 0; --d)
    {
        result[d].size = size[d];
        result[d].stride = stride;
        stride *= size[d];
    }
    return result;
}

}  // namespace detail

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
        return { m_ptr, m_shape };
    }

    operator array_ref<std::add_const_t<T>, D>() const
    {
        return as_const();
    }

    auto begin() const -> iterator
    {
        return iterator{ m_ptr, location_iterator<D>{ m_shape, location_t<D>{} } };
    }

    auto end() const -> iterator
    {
        location_t<D> last = {};
        last[D - 1] = m_shape[D - 1].size;
        return iterator{ m_ptr, location_iterator<D>{ m_shape, last } };
    }

    auto get(const location_type& loc) const -> pointer
    {
        return reinterpret_cast<pointer>(m_ptr + flat_offset(m_shape, loc));
    }

    auto operator[](const location_type& loc) const -> reference
    {
        return *get(loc);
    }

    auto slice(const slice_type& slices) const -> array_ref
    {
        const auto [new_shape, new_loc] = detail::apply_slice(m_shape, slices);
        return array_ref{ get(new_loc), new_shape };
    }

    template <class T_ = T, std::enable_if_t<!std::is_const_v<T_>, int> = 0>
    auto operator=(T_ value) -> array_ref&
    {
        std::fill(begin(), end(), value);
        return *this;
    }

    template <class T_ = T, class Range, std::enable_if_t<!std::is_const_v<T_>, int> = 0>
    void assign(Range&& range)
    {
        std::copy(std::begin(range), std::end(range), begin());
    }

    template <class T_ = T, class U, std::enable_if_t<!std::is_const_v<T_>, int> = 0>
    void assign(std::initializer_list<U> range)
    {
        std::copy(std::begin(range), std::end(range), begin());
    }

    byte* m_ptr;
    shape_type m_shape;
};

template <class T, std::size_t D>
class array
{
public:
    using value_type = T;
    using shape_type = dim_t<D>;
    using location_type = location_t<D>;
    using slice_type = slice_t<D>;
    using reference = T&;
    using pointer = T*;
    using data_type = std::vector<T>;
    using mut_ref_type = array_ref<T, D>;
    using ref_type = array_ref<const T, D>;

    array(const shape_type& shape, data_type data) : m_shape{ shape }, m_data(std::move(data))
    {
    }

    array(const size_t<D>& size) : m_shape{ detail::create_shape(size, sizeof(T)) }, m_data{}
    {
        m_data.reserve(volume(m_shape));
    }

    template <class U>
    array(const array_ref<U, D>& other) : array(size(other.shape()))
    {
        std::copy(other.begin(), other.end(), mut_ref().begin());
    }

    auto mut_ref() -> mut_ref_type
    {
        return mut_ref_type{ m_data.data(), m_shape };
    }

    auto ref() const -> ref_type
    {
        return ref_type{ m_data.data(), m_shape };
    }

    operator ref_type() const
    {
        return ref();
    }

    shape_type m_shape;
    data_type m_data;
};

namespace detail
{

struct shape_fn
{
    template <class T, std::size_t D>
    auto operator()(const array_ref<T, D>& item) const -> const dim_t<D>&
    {
        return item.m_shape;
    }

    template <class T, std::size_t D>
    auto operator()(const array<T, D>& item) const -> const dim_t<D>&
    {
        return item.m_shape;
    }

    template <std::size_t D>
    auto operator()(const dim_t<D>& item) const -> const dim_t<D>&
    {
        return item;
    }
};

struct make_array_ref_fn
{
    template <class T>
    static auto make_dim(const std::vector<T>& v) -> dim_t<1>
    {
        return dim_t<1>{ { static_cast<std::ptrdiff_t>(v.size()), sizeof(T) } };
    }

    template <class T, std::size_t N>
    static auto make_dim(const std::array<T, N>& v) -> dim_t<1>
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

    template <class T, std::size_t N>
    auto operator()(const std::array<T, N>& v) const -> array_ref<const T, 1>
    {
        return array_ref<const T, 1>{ v.data(), make_dim(v) };
    }

    template <class T, std::size_t N>
    auto operator()(std::array<T, N>& v) const -> array_ref<T, 1>
    {
        return array_ref<T, 1>{ v.data(), make_dim(v) };
    }

    template <class T, std::size_t N>
    auto operator()(T (&array)[N]) const -> array_ref<T, 1>
    {
        return array_ref<T, 1>{ array, dim_t<1>{ { static_cast<std::ptrdiff_t>(N), sizeof(T) } } };
    }
};

}  // namespace detail

static constexpr inline auto shape = adaptable<detail::shape_fn>{};
static constexpr inline auto make_array_ref = detail::make_array_ref_fn{};

}  // namespace md
}  // namespace ferrugo
