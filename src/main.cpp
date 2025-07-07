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

static constexpr inline struct let_fn
{
    template <class Func>
    constexpr auto operator()(Func&& func) const -> std::invoke_result_t<Func>
    {
        return std::invoke(std::forward<Func>(func));
    }

    template <class A0, class Func>
    constexpr auto operator()(A0&& a0, Func&& func) const -> std::invoke_result_t<Func, A0>
    {
        return std::invoke(std::forward<Func>(func), std::forward<A0>(a0));
    }

    template <class A0, class A1, class Func>
    constexpr auto operator()(A0&& a0, A1&& a1, Func&& func) const -> std::invoke_result_t<Func, A0, A1>
    {
        return std::invoke(std::forward<Func>(func), std::forward<A0>(a0), std::forward<A1>(a1));
    }

    template <class A0, class A1, class A2, class Func>
    constexpr auto operator()(A0&& a0, A1&& a1, A2&& a2, Func&& func) const -> std::invoke_result_t<Func, A0, A1, A2>
    {
        return std::invoke(std::forward<Func>(func), std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2));
    }
} let;

int run(const std::vector<std::string_view>& args)
{
    using namespace std::string_literals;
    namespace md = ferrugo::md;

#if 0

    const auto directory = "/mnt/d/Users/Krzysiek/Pictures/"s;

    auto img = md::load_bitmap(directory + "conan.bmp");

    let(img.mut_ref(),
        [](md::array_ref<md::byte, 3> ref)
        {
            let(md::shape(ref),
                [](auto shape)
                {
                    std::cout << " shape " << shape << "\n";
                    std::cout << " extents " << (shape | md::extents) << "\n";
                    std::cout << " bounds " << (shape | md::bounds) << "\n";
                    std::cout << " size " << (shape | md::size) << "\n";
                    std::cout << " volume " << (shape | md::volume) << "\n";
                });

            let(ref.slice({ //
                            md::slice_base_t{ 0, 10 },
                            md::slice_base_t{ 0, 20 },
                            md::slice_base_t{} }),
                [](auto region)
                {
                    for (const auto loc : region | md::subslice(2, 0) | md::shape | md::locations)
                    {
                        std::cout << loc << " " << (region | md::subslice(loc)) << "\n";
                    }
                });
        });

    md::save_bitmap(img.ref(), directory + "conan_out.bmp");
#endif
    auto matrix = md::array<int, 2>{ md::size_t<2>{ 3, 6 } };
    auto view = matrix.mut_ref();
    std::cout << (view | md::shape) << "\n";
    std::cout << (view | md::shape | md::volume) << "\n";
    std::iota(std::begin(view), std::end(view), 100);
    for (int y = 0; y < md::shape(view)[0].size; ++y)
    {
        for (int x = 0; x < md::shape(view)[1].size; ++x)
        {
            std::cout << view[{ y, x }] << " ";
        }
        std::cout << "\n";
    }

    std::cout << view[{ -1, -1 }] << "\n";
    std::cout << view.slice({ -1, md::slice_base_t{ -2, {} } }) << "\n";
    std::cout << view.slice({ md::slice_base_t{ -2, {} }, -1 }) << "\n";

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
