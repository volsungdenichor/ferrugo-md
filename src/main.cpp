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

void print(ferrugo::md::array_ref<int, 1> array)
{
    using namespace ferrugo::md;
    const auto shape = array.shape();
    std::cout << shape << "\n";
    std::cout << " extents " << extents(shape) << "\n";
    std::cout << " bounds " << bounds(shape) << "\n";
    std::cout << " size " << size(shape) << "\n";
    std::cout << " volume " << volume(shape) << "\n";
    for (auto loc : locations(shape))
    {
        std::cout << "  loc=" << loc << " offset=" << offset(shape, loc) << " value=" << array[loc] << "\n";
    }
    std::cout << "\n";
}

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

std::ostream& operator<<(std::ostream& os, const array_ref<const byte, 1> slice)
{
    os << "[";
    for (std::size_t n = 0; n < slice.shape()[0].size; ++n)
    {
        if (n != 0)
        {
            os << " ";
        }
        os << static_cast<int>(slice[n]);
    }
    os << "]";
    return os;
}

}  // namespace md
}  // namespace ferrugo

void run()
{
    using namespace std::string_literals;
    namespace md = ferrugo::md;

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    auto img = md::load_bitmap(directory + "hippie.bmp");
    auto copy = img.mut_ref();

    std::cout << " shape " << img.shape() << "\n";
    std::cout << " extents " << md::extents(img.shape()) << "\n";
    std::cout << " bounds " << md::bounds(img.shape()) << "\n";
    std::cout << " size " << md::size(img.shape()) << "\n";
    std::cout << " volume " << md::volume(img.shape()) << "\n";

    // apply_histogram(copy, md::equalize);

    md::save_bitmap(copy, directory + "hippie_out.bmp");

    std::cout << img.ref()[md::location_t<2>{ 0, 0 }] << "\n";

    const auto region = img.ref().slice(md::slice_t<3>{ //
                                                        md::slice_t<>{ md::_, 2, md::_ },
                                                        md::slice_t<>{ md::_, 4, md::_ },
                                                        md::slice_t<>{} });

    for (auto loc : md::locations(region.sub(2, 0).shape()))
    {
        std::cout << loc << " " << img.ref().sub_1d(2, loc) << "\n";
    }

    std::vector<float> v = { 1, 2, 3.14, 9.99, 49, 99.9 };
    md::array_ref<float, 1> ref = md::make_array_ref(v);
    for (auto val : ref.slice({ md::_, md::_, -3 }))
    {
        std::cout << val << "\n";
    }
    ref.slice({ md::_, md::_, -3 }) = -1;
    std::copy(v.begin(), v.end(), std::ostream_iterator<float>(std::cout, ", "));
    std::cout << "\n";
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
