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

template <class Exception>
double divide(int a, int b)
{
    if (b == 0)
        throw Exception{};
    return a / b;
}
} // namespace

BOOST_AUTO_TEST_SUITE(boost_leaf)

BOOST_AUTO_TEST_SUITE(try_call)

BOOST_AUTO_TEST_CASE(success_call)
{
    const auto res{xzr::error::try_call([] { return ::divide<zero_argument>(4, 2); })};

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(res.value(), 2);
}

BOOST_AUTO_TEST_CASE(std_exception_thrown)
{
    const auto res{xzr::error::try_call([] { return ::divide<zero_argument>(4, 0); })};

    BOOST_REQUIRE(!res);
}

BOOST_AUTO_TEST_CASE(no_std_exception_thrown)
{
    struct no_std_exception
    {
    };
    const auto res{xzr::error::try_call([] { return ::divide<no_std_exception>(4, 0); })};

    BOOST_REQUIRE(!res);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
