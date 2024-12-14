#pragma once

#include <ferrugo/md/array_ref.hpp>
#include <vector>

namespace ferrugo
{
namespace md
{

template <class T, std::size_t D>
class array
{
public:
    using shape_type = shape_t<D>;
    using data_type = std::vector<T>;

    array(const shape_type& shape, data_type data) : m_shape{ shape }, m_data(std::move(data))
    {
    }

    array(const array&) = default;
    array(array&&) = default;

    const shape_type& shape() const
    {
        return m_shape;
    }

    array_ref<T, D> mut_ref()
    {
        return array_ref<T, D>{ m_data.data(), m_shape };
    }

    array_ref<const T, D> ref()
    {
        return array_ref<const T, D>{ m_data.data(), m_shape };
    }

private:
    shape_type m_shape;
    data_type m_data;
};

}  // namespace md
}  // namespace ferrugo
