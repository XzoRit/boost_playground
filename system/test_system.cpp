#include <boost/test/unit_test.hpp>
#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>

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

class fixture
{
public:
    fixture() {}
    ~fixture(){}

    error_code ec{};
};

void not_ok(error_code& ec)
{
    ec = errc::make_error_code(errc::not_supported);
}

BOOST_AUTO_TEST_SUITE(boost_system)

BOOST_FIXTURE_TEST_CASE(default_constructed_error_code_is_no_error, fixture)
{
    BOOST_REQUIRE(!ec);
}

BOOST_FIXTURE_TEST_CASE(set_error_code_is_implicitly_true, fixture)
{
    not_ok(ec);

    BOOST_REQUIRE(ec);
}

BOOST_FIXTURE_TEST_CASE(error_codes_catagory_by_default_is_generic, fixture)
{
    not_ok(ec);

    BOOST_REQUIRE_EQUAL(ec.category(), generic_category());
}

BOOST_AUTO_TEST_SUITE_END()

enum app_errc
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

BOOST_AUTO_TEST_SUITE(app_error_code)

BOOST_AUTO_TEST_CASE(app_errc_construction)
{
    error_code ec{app_errc::success, app_cat};

    BOOST_REQUIRE(!ec);
}

BOOST_AUTO_TEST_SUITE_END()
