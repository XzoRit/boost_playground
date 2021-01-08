#include <try_call.hpp>

#include <boost/leaf.hpp>

#include <boost/test/unit_test.hpp>

#include <exception>

namespace leaf = boost::leaf;
namespace utf = boost::unit_test;

namespace
{
struct math_error : std::exception
{
};
struct zero_argument : math_error
{
};

double divide(int a, int b)
{
    if (b == 0)
        throw zero_argument{};
    return a / b;
}
} // namespace

BOOST_AUTO_TEST_SUITE(boost_leaf)

BOOST_AUTO_TEST_CASE(try_call)
{
    {
        const auto res{xzr::error::try_call([] { return ::divide(4, 2); })};

        BOOST_REQUIRE(res);
        BOOST_CHECK_EQUAL(res.value(), 2);
    }
    {
        const auto res{xzr::error::try_call([] { return ::divide(4, 0); })};

        BOOST_REQUIRE(!res);
    }
}

BOOST_AUTO_TEST_SUITE_END()
