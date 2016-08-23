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
    auto seq{vector<string>(num, "")};
    iota(begin(seq), end(seq), 'A');
    return seq;
}

auto append_reversed(const vector<string>& letters)
{
    auto reversed{vector<string>(next(rbegin(letters)), rend(letters))};
    reversed.insert(begin(reversed), begin(letters), end(letters));
    return reversed;
}

auto append_reversed_lines(const vector<string>& lines)
{
    auto reversed_lines{vector<string>(lines.size())};
    for(size_t i{0}; i < lines.size(); ++i)
    {
        reversed_lines[i] = lines[i] + string{next(rbegin(lines[i])), rend(lines[i])};
    }
    return reversed_lines;
}
auto insert_spaces(const vector<string>& lines)
{
    auto spaced_lines{vector<string>{}};
    for(size_t i{0}, j{lines.size() - 1}; i < lines.size(); ++i, --j)
    {
      const auto spaces_before{string(j, ' ')};
      const auto spaces_after{string(i, ' ')};
      spaced_lines.push_back(spaces_before + lines[i] + spaces_after);
    }
    return spaced_lines;
}

auto join(const vector<string>& lines)
{
    return accumulate(begin(lines), end(lines), string{""});
}
}

string create(char c)
{
    using namespace impl;
    return join(append_reversed(append_reversed_lines(insert_spaces(letters(num_of_letters(c))))));
}
}

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_diamond)

BOOST_AUTO_TEST_CASE(test_create)
{
    BOOST_TEST(diamond::create('A') == "A");
    BOOST_TEST(diamond::create('B') ==
               " A "
               "B B"
               " A ");
    BOOST_TEST(diamond::create('C') ==
               "  A  "
               " B B "
               "C   C"
               " B B "
               "  A  ");
}

BOOST_AUTO_TEST_SUITE_END()
