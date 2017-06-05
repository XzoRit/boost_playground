#include <boost/test/unit_test.hpp>

#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

#include <ostream>

namespace boost::system
{
    std::ostream& boost_test_print_type(std::ostream& str, const error_category& ec)
    {
        str << ec.name();
        return str;
    }
}

using namespace std::string_literals;
using namespace boost::system;

class boost_system_fixture
{
public:
    boost_system_fixture() {}
    ~boost_system_fixture(){}

    error_code ec{};
};

void not_ok(error_code& ec)
{
    ec = errc::make_error_code(errc::not_supported);
}

BOOST_AUTO_TEST_SUITE(boost_system)

BOOST_FIXTURE_TEST_CASE(default_constructed_error_code_is_no_error, boost_system_fixture)
{
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE_EQUAL(ec, (error_code{0, system_category()}));
}

BOOST_FIXTURE_TEST_CASE(set_error_code_is_implicitly_true, boost_system_fixture)
{
    not_ok(ec);

    BOOST_REQUIRE(ec);
}

BOOST_FIXTURE_TEST_CASE(error_codes_catagory_by_default_is_generic, boost_system_fixture)
{
    not_ok(ec);

    BOOST_REQUIRE_EQUAL(ec.category(), generic_category());
}

BOOST_AUTO_TEST_SUITE_END()
