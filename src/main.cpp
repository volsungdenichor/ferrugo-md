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

namespace ferrugo
{

namespace md_v3
{

void apply_histogram(
    array_ref<byte, 3> dest,
    array_ref<const byte, 3> source,  //
    const std::function<transform_fn(const histogram_t&)>& func)
{
    for (int c = 0; c < 3; ++c)
    {
        const auto channel = slice_t<3>{ slice_t<>{}, slice_t<>{}, at(c) };
        const auto source_channel = source.slice(channel);
        const auto dest_channel = dest.slice(channel);

        const auto histogram = make_histogram(source_channel);
        const auto lut = lut_t{ func(histogram) };

        std::transform(std::begin(source_channel), std::end(source_channel), std::begin(dest_channel), lut);
    }
}

void apply_histogram(
    array_ref<byte, 3> image,  //
    const std::function<transform_fn(const histogram_t&)>& func)
{
    apply_histogram(image, image.as_const(), func);
}

}  // namespace md_v3
}  // namespace ferrugo

void run()
{
    using namespace std::string_literals;
    namespace md = ferrugo::md_v3;

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    auto img = md::load_bitmap(directory + "hippie.bmp");
    auto copy = img.mut_ref();

    std::cout << " shape " << img.shape() << "\n";
    std::cout << " extents " << md::extents(img.shape()) << "\n";
    std::cout << " bounds " << md::bounds(img.shape()) << "\n";
    std::cout << " size " << md::size(img.shape()) << "\n";
    std::cout << " volume " << md::volume(img.shape()) << "\n";

    apply_histogram(copy, md::equalize);

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
