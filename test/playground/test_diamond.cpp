#include <string>

using namespace std;
using namespace std::literals::string_literals;

namespace diamond
{
string create(char c);
}

#include <algorithm>
#include <iterator>

namespace diamond
{
namespace impl
{
auto num_of_letters(char c)
{
    return c - 'A' + 1;
}

auto letters(int num)
{
    return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"s.substr(0, num);
}

auto append_reversed(const std::string letters)
{
  return letters + string{next(rbegin(letters)), rend(letters)};
}
}
string create(char c)
{
    using namespace impl;
    return append_reversed(letters(num_of_letters(c)));
}
}

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(diamond)

BOOST_AUTO_TEST_CASE(test_diamond)
{
    BOOST_TEST(diamond::create('A') == "A");
    BOOST_TEST(diamond::create('B') == "ABA");
    BOOST_TEST(diamond::create('C') == "ABCBA");
}

BOOST_AUTO_TEST_SUITE_END()
