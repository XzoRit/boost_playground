#include "app_error_codes.hpp"
#include "lib_error_codes.hpp"

#include <boost/test/unit_test.hpp>

#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

using namespace std::string_literals;
using namespace boost::system;

class my_error_code_fixture
{
public:
    my_error_code_fixture() {}
    ~my_error_code_fixture(){}
};

BOOST_AUTO_TEST_SUITE(my_error_code)

BOOST_FIXTURE_TEST_CASE(my_errc_construction, my_error_code_fixture)
{
    BOOST_REQUIRE(!app::ec_succ);
    BOOST_REQUIRE( app::ec_fail);
    BOOST_REQUIRE_EQUAL(app::ec_succ.message(), "app error: 0");
    BOOST_REQUIRE_EQUAL(app::ec_fail.message(), "app error: 1");
}

BOOST_FIXTURE_TEST_CASE(my_errc_comparison, my_error_code_fixture)
{
    BOOST_REQUIRE_EQUAL(app::ec_succ, make_error_code(app::error::success));
    BOOST_REQUIRE_EQUAL(app::ec_fail, make_error_code(app::error::failure));
    BOOST_REQUIRE_NE(app::ec_succ, app::ec_fail);

    BOOST_REQUIRE_NE(lib::ec_succ, app::ec_succ);
    BOOST_REQUIRE_NE(lib::ec_succ, app::ec_fail);
    BOOST_REQUIRE_NE(lib::ec_fail, app::ec_succ);
    BOOST_REQUIRE_NE(lib::ec_fail, app::ec_fail);
}

BOOST_FIXTURE_TEST_CASE(my_errcs_are_recognized_by_stl, my_error_code_fixture)
{
    static_assert(is_error_code_enum<app::error::type>::value, "");
    static_assert(is_error_code_enum<lib::error::type>::value, "");
}

BOOST_FIXTURE_TEST_CASE(my_errcs_can_be_thrown_as_system_error, my_error_code_fixture)
{
    BOOST_REQUIRE_EXCEPTION(
        throw system_error(app::ec_fail),
        system_error,
        [](const auto& e){ return e.code() == app::ec_fail; });
    BOOST_REQUIRE_EXCEPTION(
        throw system_error(lib::ec_fail),
        system_error,
        [](const auto& e){ return e.code() == lib::ec_fail; });
}

BOOST_AUTO_TEST_SUITE_END()
