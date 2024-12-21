#include <array>
#include <ferrugo/md_v2/access.hpp>
#include <ferrugo/md_v2/array_ref.hpp>
#include <ferrugo/md_v2/shape_iterator.hpp>
#include <ferrugo/md_v2/slice.hpp>
#include <ferrugo/md_v2/types.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

void run()
{
    namespace md = ferrugo::md_v2;
    std::vector<unsigned char> data(1000);
    std::iota(std::begin(data), std::end(data), 0);

#if 0
    md::dim_t<3> shape{ md::dim_t<>{ 3, 4 * 3 }, md::dim_t<>{ 4, 3 }, md::dim_t<>{ 3, 1 } };
    const auto a = md::array_ref<const unsigned char, 3>{ data.data(), shape };
#else
    const auto a = md::array_ref<const unsigned char, 1>{ data.data(), md::dim_t<1>{ 5, 2 } };
#endif
    std::cout << "--\n";
    for (const auto& p : a)
    {
        std::cout << (int)p << "\n";
    }
    std::cout << "--\n";
    for (const auto& p : a.slice({ {}, {}, -1 }))
    {
        std::cout << (int)p << "\n";
    }
    std::cout << "--\n";
    for (const auto& p : a.slice({ {}, {}, -1 }).slice({ {}, {}, -1 }))
    {
        std::cout << (int)p << "\n";
    }
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
