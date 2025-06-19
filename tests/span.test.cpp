#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <ferrugo/core/ostream_utils.hpp>
#include <ferrugo/core/std_ostream.hpp>
#include <ferrugo/md/array.hpp>

namespace std
{
ostream& operator<<(ostream& os, byte item)
{
    return os << static_cast<int>(item);
}
}  // namespace std

using namespace ferrugo;
using namespace std::string_literals;

TEST_CASE("shape", "[md]")
{
}

TEST_CASE("array_ref 2d", "[md]")
{
}

TEST_CASE("array_ref - indices", "[md]")
{
}

TEST_CASE("array_ref", "[md]")
{
}
