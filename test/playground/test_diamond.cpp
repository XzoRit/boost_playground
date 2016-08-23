#include <string>

using namespace std::literals::string_literals;

namespace diamond
{
namespace impl
{
int amount_from_char(char c);
std::string letters(int amount);
}
std::string create(char c);
}

namespace diamond
{
namespace impl
{
int amount_from_char(char c)
{
  return c - 'A' + 1;
}

std::string letters(int amount)
{
  return "ABCDEFGHIJKLMNOPQRSTUVWXYZ"s.substr(0, amount);
}
}
std::string create(char c)
{
  using namespace impl;
  return letters(amount_from_char(c));
}
}

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(diamond)

BOOST_AUTO_TEST_CASE(test_diamond)
{
  BOOST_TEST(diamond::create('A') == "A");
  BOOST_TEST(diamond::create('B') == "AB");
  BOOST_TEST(diamond::create('C') == "ABC");
}

BOOST_AUTO_TEST_SUITE_END()
