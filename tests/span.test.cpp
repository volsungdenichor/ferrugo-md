#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/md/array_ref.hpp>
#include <ferrugo/md/shape.hpp>

namespace std
{
ostream& operator<<(ostream& os, byte item)
{
    return os << static_cast<int>(item);
}
}  // namespace std

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

using image = md::array_ref<std::byte, 3>;
using image_channel = md::array_ref<std::byte, 2>;

auto create_image_shape(md::index_t h, md::index_t w) -> image::shape_type
{
    return image::shape_type(md::dim_t{ h, 3 * w }, md::dim_t{ w, 3 }, md::dim_t{ 3, 1 });
}

TEST_CASE("array_ref", "[md]")
{
    const auto shape = create_image_shape(4, 6);
    std::vector<std::byte> v;
    v.resize(md::volume(shape));
    const auto a = image{ v.data(), shape };
    a[md::location_t<2>(1, 1)] = { std::byte{ 255 }, std::byte{ 128 }, std::byte{ 64 } };
    a[md::location_t<2>(2, 2)] = { std::byte(25), std::byte(35), std::byte(45) };
    a[md::location_t<2>(3, 1)] = { std::byte(99), std::byte(77), std::byte(55) };

    for (md::array_ref<std::byte, 2> x : md::slices(0)(a))
    {
        std::cout << "ROW " << x << "\n";
        for (md::array_ref<std::byte, 1> y : md::slices(0)(x))
        {
            std::cout << "  " << y << "\n";
        }
    }

    // for (auto x : md::subslices(2)(a))
    // {
    //     std::cout << " - " << x << std::endl;
    // }
}