#pragma once

#include <ferrugo/md_v3/array_ref.hpp>

namespace ferrugo
{
namespace md_v3
{

template <std::size_t D>
auto create_shape(const size_t<D>& size) -> dim_t<D>
{
    dim_t<D> result = {};
    location_base_t stride = 1;
    for (int d = D - 1; d >= 0; --d)
    {
        result[d].size = size[d];
        result[d].stride = stride;
        result[d].min = 0;
        stride *= size[d];
    }
    return result;
}

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

    array(const size_t<D>& size) : m_shape{ create_shape(size) }, m_data{}
    {
        m_data.reserve(volume(m_shape));
    }

    template <class U>
    array(const array_ref<U, D>& other) : array(size(other.shape()))
    {
        std::copy(other.begin(), other.end(), mut_ref().begin());
    }

    const shape_type& shape() const
    {
        return m_shape;
    }

    auto mut_ref() -> mut_ref_type
    {
        return mut_ref_type{ m_data.data(), m_shape };
    }

    auto ref() const -> ref_type
    {
        return ref_type{ m_data.data(), m_shape };
    }

private:
    shape_type m_shape;
    data_type m_data;
};

}  // namespace md_v3
}  // namespace ferrugo
