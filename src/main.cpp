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
        const auto channel = slice_t<3>{ slice_base_t{}, slice_base_t{}, c };
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

template <class T>
struct format
{
    template <class U>
    void operator()(std::ostream& os, U&& item) const
    {
        os << std::forward<U>(item);
    }
};

template <>
struct format<md::byte>
{
    void operator()(std::ostream& os, md::byte item) const
    {
        os << static_cast<int>(item);
    }
};

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const array_ref<T, N> item)
{
    static const auto fmt = format<std::remove_const_t<T>>{};
    os << "[";
    auto it = item.begin();
    const auto e = item.end();
    if (it != e)
    {
        fmt(os, *it++);
    }
    for (; it != e; ++it)
    {
        os << " ";
        fmt(os, *it);
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

    auto img = md::load_bitmap(directory + "conan.bmp");

    std::cout << " shape " << img.shape() << "\n";
    std::cout << " extents " << md::extents(img.shape()) << "\n";
    std::cout << " bounds " << md::bounds(img.shape()) << "\n";
    std::cout << " size " << md::size(img.shape()) << "\n";
    std::cout << " volume " << md::volume(img.shape()) << "\n";

    auto region = img.mut_ref().slice({ //
                                        md::slice_base_t{ 105, md::_, 3 },
                                        md::slice_base_t{ 100, md::_, 2 },
                                        md::slice_base_t{ 0, 1 } });

    region = 25;

    img.mut_ref().slice({ 0, md::slice_base_t{}, 0 }) = 255;
    img.mut_ref().slice({ 0, md::slice_base_t{}, 1 }) = 0;
    img.mut_ref().slice({ 0, md::slice_base_t{}, 2 }) = 10;

    img.mut_ref().slice({ -1, md::slice_base_t{}, 0 }) = 255;
    img.mut_ref().slice({ -1, md::slice_base_t{}, 1 }) = 0;
    img.mut_ref().slice({ -1, md::slice_base_t{}, 2 }) = 10;

    img.mut_ref().slice({ md::slice_base_t{}, 0, 0 }) = 255;
    img.mut_ref().slice({ md::slice_base_t{}, 0, 1 }) = 0;
    img.mut_ref().slice({ md::slice_base_t{}, 0, 2 }) = 0;

    md::save_bitmap(img.ref(), directory + "conan_out.bmp");

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
