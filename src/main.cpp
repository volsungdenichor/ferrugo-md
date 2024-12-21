#include <array>
#include <ferrugo/md_v3/access.hpp>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

void run()
{
    namespace md = ferrugo::md_v3;
    // const auto b = md::bounds_t<3>(md::bounds_t<>{ 1, 5 }, md::bounds_t<>{ 2, 6 }, md::bounds_t<>{ 3, 7 });
    const auto b = md::dim_t<>(10, 2);
    const auto s = md::slice_t<>(md::_, md::_, -1);
    std::cout << b << "\n";
    std::cout << s << "\n";
    // std::cout << md::min(b) << "\n";
    std::cout << md::max(b) << "\n";
    // std::cout << md::lower(b) << "\n";
    // std::cout << md::upper(b) << "\n";
    // std::cout << md::size(b) << "\n";
    // std::cout << md::bounds(b) << "\n";
    std::cout << md::apply_slice(b, s) << "\n";
    std::cout << md::apply_slice(md::apply_slice(b, s), s) << "\n";
    // std::cout << md::stride(b) << "\n";
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
