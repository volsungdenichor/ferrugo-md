#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/core/std_ostream.hpp>
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

TEST_CASE("array_ref - indices", "[md]")
{
    const auto s = md::slice_t{ {}, {}, 2 };
    std::cout << core::delimit(md::indices(s, 10), " ") << "\n";
}

TEST_CASE("array_ref", "[md]")
{
    using byte = ferrugo::md::byte;

    auto arr = ferrugo::md::load_bitmap(R"(/mnt/d/Users/Krzysiek/Pictures/hippie.bmp)");
    std::cout << md::extents(arr.shape()) << "\n";
    auto a = arr.mut_ref();
    auto b = md::transpose(a)[{ md::slice_t{ {}, {}, -1 }, md::slice_t{}, md::slice_t{} }];
    b[50][200] = { 255, 0, 0 };
    ferrugo::md::save_bitmap(b.as_const(), R"(/mnt/d/Users/Krzysiek/Pictures/hippie_ooo.bmp)");
}
