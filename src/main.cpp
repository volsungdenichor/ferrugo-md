#include <array>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/md_v3/access.hpp>
#include <ferrugo/md_v3/array_ref.hpp>
#include <ferrugo/md_v3/shape_iterator.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

void print(ferrugo::md_v3::array_ref<int, 1> array)
{
    const auto shape = array.shape();
    std::cout << shape << "\n";
    std::cout << "min " << ferrugo::md_v3::min(shape) << "\n";
    std::cout << "max " << ferrugo::md_v3::max(shape) << "\n";
    std::cout << "lower " << ferrugo::md_v3::lower(shape) << "\n";
    std::cout << "upper " << ferrugo::md_v3::upper(shape) << "\n";
    std::cout << "bounds " << ferrugo::md_v3::bounds(shape) << "\n";
    std::cout << "size " << ferrugo::md_v3::size(shape) << "\n";
    for (auto loc : ferrugo::md_v3::iter(shape))
    {
        std::cout << "loc" << loc << " offset=" << ferrugo::md_v3::offset(shape, loc) << " value=" << array[loc] << "\n";
    }
    std::cout << "\n";
}

void run()
{
    namespace md = ferrugo::md_v3;
    std::vector<int> data;
    data.resize(100);
    std::iota(data.begin(), data.end(), 0);
    md::array_ref<int, 1> array{ data.data(), md::dim_t<1>{ 10, sizeof(int), 0 } };
    print(array);
    const auto s = array.slice(md::slice_t<>{ 8, 3, -1 });
    print(s);
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
