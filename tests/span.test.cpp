#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/md/array_ref.hpp>
#include <ferrugo/md/shape.hpp>

using namespace ferrugo;
using namespace std::string_literals;

TEST_CASE("shape", "[md]")
{
    const auto shape = md::shape_t<2>(md::dim_t{ 5, 1 }, md::dim_t{ 4, 5 });
    // std::cout << shape << std::endl;
    // std::cout << md::lower(shape) << std::endl;
    // std::cout << md::upper(shape) << std::endl;
    // std::cout << md::min(shape) << std::endl;
    // std::cout << md::max(shape) << std::endl;
    // std::cout << md::size(shape) << std::endl;
    // std::cout << md::stride(shape) << std::endl;
    // std::cout << md::offset(shape, md::location_t<2>{ 0, 0 }) << std::endl;
    // std::cout << md::offset(shape, md::location_t<2>{ 0, 1 }) << std::endl;
    // std::cout << md::offset(shape, md::location_t<2>{ 0, 2 }) << std::endl;
    // std::cout << md::volume(shape) << std::endl;
}

TEST_CASE("array_ref 2d", "[md]")
{
    const auto shape = md::shape_t<2>(md::dim_t{ 5, sizeof(int) }, md::dim_t{ 3, 5 * sizeof(int) });
    std::vector<int> data(15);
    for (int i = 0; i < data.size(); ++i)
    {
        data[i] = i;
    }
    const auto array = md::array_ref<int, 2>{ data.data(), shape };

    std::cout << core::delimit(array, " ") << "\n\n";
    std::cout << core::delimit(md::slices(0)(array), "\n") << "\n\n";
    std::cout << core::delimit(md::slices(1)(array), "\n") << "\n\n";
}

TEST_CASE("array_ref", "[md]")
{
    const auto shape = md::shape_t<3>(md::dim_t{ 5, 3 }, md::dim_t{ 4, 5 * 3 }, md::dim_t{ 3, 1 });
    std::cout << shape << std::endl;
    std::vector<unsigned char> v;
    v.resize(md::volume(shape));
    const auto a = md::array_ref<unsigned char, 3>{ v.data(), shape };
    a[md::location_t<3>{ 0, 0, 0 }] = 255;
    a[md::location_t<3>{ 0, 0, 1 }] = 128;
    a[md::location_t<3>{ 0, 0, 2 }] = 64;

    a[md::location_t<2>{ 0, 0 }] = 77;
    a[md::location_t<3>{ 0, 0, 1 }] = 99;
    std::cout << a[md::location_t<2>{ 0, 0 }] << std::endl;

    a[md::location_t<3>{ 1, 0, 0 }] = 9;
    a[md::location_t<3>{ 1, 0, 1 }] = 9;
    a[md::location_t<3>{ 1, 0, 2 }] = 9;

    a[md::location_t<3>{ 2, 2, 0 }] = 11;
    a[md::location_t<3>{ 2, 2, 1 }] = 22;
    a[md::location_t<3>{ 2, 2, 2 }] = 33;

    for (auto x : md::slices(0)(a))
    {
        for (auto y : md::slices(0)(x))
        {
            std::cout << y << std::endl;
        }
    }

    for (auto x : md::subslices(2)(a))
    {
        std::cout << " - " << x << std::endl;
    }
}