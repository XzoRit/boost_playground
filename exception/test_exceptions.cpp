#include <boost/test/unit_test.hpp>
#include <boost/exception/all.hpp>
#include <stdexcept>
#include <string>

class exception_base : public virtual std::exception
                     , public virtual boost::exception {};

class error_a        : public virtual exception_base {};
class error_b        : public virtual exception_base {};
class error_c        : public virtual exception_base {};

typedef boost::error_info<struct tag_error_code, int> error_code;

void c() { throw error_c() << error_code(666); }

typedef boost::error_info<struct tag_error_string, std::string> error_string;

void b()
{
    try
    {
        c();
    }
    catch(const boost::exception& e)
    {
        e << error_string("b() was called :-)");
        throw;
    }
}

typedef boost::error_info<struct tag_another_error_code, int> another_error_code;

void a()
{
    try
    {
        b();
    }
    catch(const boost::exception& e)
    {
        e << another_error_code(1234);
        throw;
    }
}

namespace utf = boost::unit_test;

bool contains_error_infos(const error_c& e)
{
    BOOST_TEST((*boost::get_error_info<error_code>(e)) == 666);
    BOOST_TEST((*boost::get_error_info<error_string>(e)) == "b() was called :-)");
    BOOST_TEST((*boost::get_error_info<another_error_code>(e)) == 1234);
    return true;
}

BOOST_AUTO_TEST_SUITE(exceptions)

BOOST_AUTO_TEST_CASE(test_1)
{
    BOOST_CHECK_EXCEPTION(a(), error_c, contains_error_infos);
}

BOOST_AUTO_TEST_SUITE_END()
