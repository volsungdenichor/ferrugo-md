#include <array>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/md/access.hpp>
#include <ferrugo/md/array.hpp>
#include <ferrugo/md/bitmap.hpp>
#include <ferrugo/md/histogram.hpp>
#include <ferrugo/md/lut.hpp>
#include <ferrugo/md/transformations.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

namespace ferrugo
{

namespace md
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

template <std::size_t N>
std::ostream& operator<<(std::ostream& os, const array_ref<const byte, N> item)
{
    os << "[";
    auto it = item.begin();
    const auto e = item.end();
    if (it != e)
    {
        os << static_cast<int>(*it++);
    }
    for (; it != e; ++it)
    {
        os << " ";
        os << static_cast<int>(*it);
    }
    os << "]";
    return os;
}

}  // namespace md
}  // namespace ferrugo

int run(const std::vector<std::string_view>& args)
{
    using namespace std::string_literals;
    namespace md = ferrugo::md;

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    const auto img = md::load_bitmap(directory + "hippie.bmp");

    std::cout << " shape " << img.shape() << "\n";
    std::cout << " extents " << md::extents(img.shape()) << "\n";
    std::cout << " bounds " << md::bounds(img.shape()) << "\n";
    std::cout << " size " << md::size(img.shape()) << "\n";
    std::cout << " volume " << md::volume(img.shape()) << "\n";

    const auto region = img.ref().slice(md::slice_t<3>{ //
                                                        0,
                                                        -1,
                                                        md::slice_t<>{} });

    std::cout << region.shape() << "\n";
    std::cout << md::size(region.shape()) << "\n";
    std::cout << region << "\n";

    return 0;
}

int main(int argc, char** argv)
{
    try
    {
        return run(std::vector<std::string_view>(argv, argv + argc));
    }
    catch (const std::exception& ex)
    {
        std::cerr << "\n"
                  << "Error:"
                  << "\n"
                  << ex.what();
    }
}
