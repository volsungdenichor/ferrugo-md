#include <array>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/array.hpp>
#include <ferrugo/md_v3/bitmap.hpp>
#include <ferrugo/md_v3/shape_iterator.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

void print(ferrugo::md_v3::array_ref<int, 1> array)
{
    using namespace ferrugo::md_v3;
    const auto shape = array.shape();
    std::cout << shape << "\n";
    std::cout << " extents " << extents(shape) << "\n";
    std::cout << " bounds " << bounds(shape) << "\n";
    std::cout << " size " << size(shape) << "\n";
    std::cout << " volume " << volume(shape) << "\n";
    for (auto loc : iter(shape))
    {
        std::cout << "  loc=" << loc << " offset=" << offset(shape, loc) << " value=" << array[loc] << "\n";
    }
    std::cout << "\n";
}

void run()
{
    using namespace std::string_literals;
    namespace md = ferrugo::md_v3;

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    auto img = md::load_bitmap(directory + "hippie.bmp");
    auto copy = img.mut_ref().slice(md::slice_t<3>(
        md::slice_t<>{ md::_, -100, md::_ }, md::slice_t<>{ md::_, md::_, -1 }, md::slice_t<>{ md::_, md::_, md::_ }));

    for (auto loc : md::iter(copy.shape()))
    {
        copy[loc] = 255 - copy[loc];
    }

    md::save_bitmap(copy, directory + "hippie_out.bmp");
}

int main()
{
    try
    {
        run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "\n"
                  << "Error:"
                  << "\n"
                  << ex.what();
    }
}
