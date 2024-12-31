#pragma once

#include <ferrugo/md_v3/array_ref.hpp>

namespace ferrugo
{
namespace md_v3
{

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
