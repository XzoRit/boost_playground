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

enum class app_errc
{
    success = 0,
    failure
};

class app_cat_t : public error_category
{
public:
    const char* name() const noexcept override
    {
        return "app";
    }

    std::string message(int ev) const noexcept override
    {
        return "app error: "s + std::to_string(ev);
    }
};

const app_cat_t app_cat;

error_code make_error_code(app_errc ec)
{
    return error_code(static_cast<int>(ec), app_cat);
}

namespace boost::system
{
    template<>
    struct is_error_code_enum<app_errc> : std::true_type
    {};
}

const error_code ec_app_succ{make_error_code(app_errc::success)};
const error_code ec_app_fail{make_error_code(app_errc::failure)};

enum class lib_errc
{
    success = 0,
    failure
};

class lib_cat_t : public error_category
{
public:
    const char* name() const noexcept override
    {
        return "lib";
    }

    std::string message(int ev) const noexcept override
    {
        return "lib error: "s + std::to_string(ev);
    }
};

const lib_cat_t lib_cat;

error_code make_error_code(lib_errc ec)
{
    return error_code(static_cast<int>(ec), lib_cat);
}

namespace boost::system
{
    template<>
    struct is_error_code_enum<lib_errc> : std::true_type
    {};
}

const error_code ec_lib_succ{make_error_code(lib_errc::success)};
const error_code ec_lib_fail{make_error_code(lib_errc::failure)};

class my_error_code_fixture
{
public:
    my_error_code_fixture() {}
    ~my_error_code_fixture(){}
};

BOOST_AUTO_TEST_SUITE(my_error_code)

BOOST_FIXTURE_TEST_CASE(my_errc_construction, my_error_code_fixture)
{
    BOOST_REQUIRE(!ec_app_succ);
    BOOST_REQUIRE( ec_app_fail);
    BOOST_REQUIRE_EQUAL(ec_app_succ.message(), "app error: 0");
    BOOST_REQUIRE_EQUAL(ec_app_fail.message(), "app error: 1");
}

BOOST_FIXTURE_TEST_CASE(my_errc_comparison, my_error_code_fixture)
{
    BOOST_REQUIRE_EQUAL(ec_app_succ, make_error_code(app_errc::success));
    BOOST_REQUIRE_EQUAL(ec_app_fail, make_error_code(app_errc::failure));
    BOOST_REQUIRE_NE(ec_app_succ, ec_app_fail);

    BOOST_REQUIRE_NE(ec_lib_succ, ec_app_succ);
    BOOST_REQUIRE_NE(ec_lib_succ, ec_app_fail);
    BOOST_REQUIRE_NE(ec_lib_fail, ec_app_succ);
    BOOST_REQUIRE_NE(ec_lib_fail, ec_app_fail);
}

BOOST_FIXTURE_TEST_CASE(my_errcs_are_recognized_by_stl, my_error_code_fixture)
{
    static_assert(is_error_code_enum<app_errc>::value, "");
    static_assert(is_error_code_enum<lib_errc>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()
