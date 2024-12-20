#include <array>
#include <ferrugo/md_v2/access.hpp>
#include <ferrugo/md_v2/types.hpp>
#include <iostream>
#include <memory>
#include <vector>

void run()
{
    namespace md = ferrugo::md_v2;
    md::bounds_t<2> item{ md::bounds_t<>{ 10, 15 }, md::bounds_t<>{ 5, 10 } };
    std::cout << item << "\n";
    std::cout << md::min(item) << "\n";
    std::cout << md::max(item) << "\n";
    std::cout << md::lower(item) << "\n";
    std::cout << md::upper(item) << "\n";
    std::cout << md::size(item) << "\n";
    std::cout << md::bounds(item) << "\n";
    std::cout << md::volume(item) << "\n";
    std::cout << md::contains(item, md::location_t<2>{ 3, 3 }) << "\n";
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
