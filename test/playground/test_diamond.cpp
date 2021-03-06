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
const auto num_of_letters = [](auto c)
{
    return c - 'A' + 1;
};

const auto letters = [](auto num)
{
    auto seq{vector<string>(num, "")};
    iota(begin(seq), end(seq), 'A');
    return seq;
};

const auto mirror_horizontal = [](const auto& letters)
{
    auto reversed{vector<string>{next(rbegin(letters)), rend(letters)}};
    reversed.insert(begin(reversed), begin(letters), end(letters));
    return reversed;
};

const auto mirror_vertical = [](const auto& lines)
{
    auto reversed_lines{vector<string>{lines.size()}};
    transform(
        begin(lines), end(lines),
        begin(reversed_lines),
        [](auto a)

    {
        return a + string{next(rbegin(a)), rend(a)};
    });
    return reversed_lines;
};

const auto insert_spaces = [](const auto& lines)
{
    auto spaced_lines{vector<string>{}};
    for(size_t i{0}, j{lines.size() - 1}; i < lines.size(); ++i, --j)
    {
        const auto spaces_before{string(j, ' ')};
        const auto spaces_after{string(i, ' ')};
        spaced_lines.push_back(spaces_before + lines[i] + spaces_after);
    }
    return spaced_lines;
};

const auto join = [](const auto& lines)
{
    return accumulate(
               begin(lines), end(lines),
               string{""},
               [](auto a, auto b)
    {
        return a + b + '\n';
    });
};
}

string create(char c)
{
    using namespace impl;
    return join(
               mirror_horizontal(
                   mirror_vertical(
                       insert_spaces(
                           letters(
                               num_of_letters(c))))));
}
}

#include <boost/test/unit_test.hpp>

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_diamond)

BOOST_AUTO_TEST_CASE(test_create)
{
    BOOST_TEST(diamond::create('A') ==
               "A\n");
    BOOST_TEST(diamond::create('B') ==
               " A \n"
               "B B\n"
               " A \n");
    BOOST_TEST(diamond::create('C') ==
               "  A  \n"
               " B B \n"
               "C   C\n"
               " B B \n"
               "  A  \n");
}

BOOST_AUTO_TEST_SUITE_END()
