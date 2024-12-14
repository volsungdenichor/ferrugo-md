#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/md/array.hpp>
#include <ferrugo/md/bitmap.hpp>

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

TEST_CASE("array_ref", "[md]")
{
    using byte = ferrugo::md::byte;

    auto arr = ferrugo::md::load_bitmap(R"(/mnt/d/Users/Krzysiek/Pictures/hippie.bmp)");
    auto a = arr.mut_ref();
    std::cout << arr.shape() << "\n";
    a[md::location_t<2>{ 5, 5 }] = { 255, 255, 0 };
    a[md::location_t<2>{ 10, 10 }] = { 255, 255, 0 };
    a[md::location_t<2>{ 15, 15 }] = { 255, 255, 0 };
    ferrugo::md::save_bitmap(arr.ref().slice(2, 0), R"(/mnt/d/Users/Krzysiek/Pictures/hippie_ooo.bmp)");
}