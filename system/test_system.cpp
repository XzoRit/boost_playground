#include <boost/test/unit_test.hpp>
#include <boost/system/error_code.hpp>

using namespace boost::system;

BOOST_AUTO_TEST_SUITE(boost_system)

void not_ok(error_code& ec)
{
    ec = errc::make_error_code(errc::not_supported);
}

BOOST_AUTO_TEST_CASE(default_constructed_error_code_is_no_error)
{
    error_code ec{};
    BOOST_REQUIRE(!ec);
}

BOOST_AUTO_TEST_CASE(set_error_code_is_implicitly_true)
{
    error_code ec{};

    not_ok(ec);

    BOOST_REQUIRE(ec);
}

BOOST_AUTO_TEST_SUITE_END()
