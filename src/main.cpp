#include <array>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/array.hpp>
#include <ferrugo/md_v3/bitmap.hpp>
#include <ferrugo/md_v3/histogram.hpp>
#include <ferrugo/md_v3/lut.hpp>
#include <ferrugo/md_v3/shape_iterator.hpp>
#include <ferrugo/md_v3/transformations.hpp>
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

void apply(
    ferrugo::md_v3::array_ref<const ferrugo::md_v3::byte, 3> source,
    ferrugo::md_v3::array_ref<ferrugo::md_v3::byte, 3> dest,
    const std::function<ferrugo::md_v3::transform_fn(const ferrugo::md_v3::histogram_t&)>& func)
{
    for (int c = 0; c < 3; ++c)
    {
        const auto source_channel = source.subslice(2, c);
        const auto dest_channel = dest.subslice(2, c);

        const auto histogram = ferrugo::md_v3::make_histogram(source_channel);
        const auto transform = func(histogram);

        const auto lut = ferrugo::md_v3::lut_t{ transform };

        std::transform(std::begin(source_channel), std::end(source_channel), std::begin(dest_channel), lut);
    }
}

void run()
{
    using namespace std::string_literals;
    namespace md = ferrugo::md_v3;

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    auto img = md::load_bitmap(directory + "hippie.bmp");
    auto copy = img.mut_ref();

    // const auto lut = md::lut_t{ md::stretch(histogram) };

    // std::transform(std::begin(copy), std::end(copy), std::begin(copy), lut);

    apply(copy.as_const(), copy, ferrugo::md_v3::equalize);

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
