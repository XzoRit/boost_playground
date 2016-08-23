#include <string>

using namespace std;
using namespace std::literals::string_literals;

namespace diamond
{
string create(char c);
}

#include <algorithm>
#include <iterator>
#include <vector>
#include <numeric>

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

  auto append_reversed(const vector<string>& letters)
{
  auto reversed{vector<string>(next(rbegin(letters)), rend(letters))};
  reversed.insert(begin(reversed), begin(letters), end(letters));
  return reversed;
}

  auto double_letters(const std::string& letters)
{
    auto doubled{vector<string>{"A"}};
    if(letters.size() == 1) return doubled;
    for(auto&& c : string{next(begin(letters)), end(letters)})
    {
      doubled.push_back(string(2, c));
    }
    return doubled;
}

  auto join(const vector<string>& lines)
  {
    return accumulate(begin(lines), end(lines), string{""});
  }
}

string create(char c)
{
    using namespace impl;
    return join(append_reversed(double_letters(letters(num_of_letters(c)))));
}
}

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_diamond)

BOOST_AUTO_TEST_CASE(test_create)
{
    BOOST_TEST(diamond::create('A') == "A");
    BOOST_TEST(diamond::create('B') == "ABBA");
    BOOST_TEST(diamond::create('C') == "ABBCCBBA");
}

BOOST_AUTO_TEST_CASE(test_double_letters)
{
  BOOST_TEST(diamond::impl::double_letters("A")   == (vector<string>{"A"}));
  BOOST_TEST(diamond::impl::double_letters("AB")  == (vector<string>{"A", "BB"}));
  BOOST_TEST(diamond::impl::double_letters("ABC") == (vector<string>{"A", "BB", "CC"}));
}

BOOST_AUTO_TEST_SUITE_END()
