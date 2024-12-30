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
    namespace md = ferrugo::md_v3;
    std::vector<int> data;
    data.resize(100);
    std::iota(data.begin(), data.end(), 0);
    md::array_ref<int, 1> array{ data.data(), md::dim_t<1>{ 10, sizeof(int), 0 } };
    print(array);
    const auto s0 = array.slice(md::slice_t<>{ 1, 8, 2 });
    print(s0);
    const auto s1 = s0.slice(md::slice_t<>{ md::_, md::_, -2 });
    print(s1);
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
