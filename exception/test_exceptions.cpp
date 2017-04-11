#include <boost/test/unit_test.hpp>
#include <boost/exception/all.hpp>
#include <stdexcept>
#include <string>
#include <iostream>

class current_location
{
public:
    current_location(const char* func_name, const char* file, int line)
        : m_func_name(func_name)
        , m_file(file)
        , m_line(line)
    {}

    const char* func() const { return m_func_name; }
    const char* file() const { return m_file     ; }
    int         line() const { return m_line     ; }
private:
    const char* m_func_name;
    const char* m_file;
    int m_line;
};

std::ostream& operator<<(std::ostream& str, const current_location& loc)
{
    str << loc.file() << '(' << loc.line() << "): in function " << loc.func();
    return str;
}

#define CURRENT_LOCATION() \
    current_location(BOOST_THROW_EXCEPTION_CURRENT_FUNCTION, __FILE__, __LINE__)

#define ADD_EXCEPTION_INFO(x)\
        ::boost::enable_error_info(x) <<\
        ::boost::throw_function(BOOST_THROW_EXCEPTION_CURRENT_FUNCTION) <<\
        ::boost::throw_file(__FILE__) <<\
        ::boost::throw_line((int)__LINE__)

class exception_base : public virtual std::exception
                     , public virtual boost::exception {};

class error_a        : public virtual exception_base {};
class error_b        : public virtual exception_base {};
class error_c        : public virtual exception_base {};

typedef boost::error_info<struct tag_error_code, int> error_code;

bool c(bool ok)
{
    if(!ok) throw ADD_EXCEPTION_INFO(error_c()) << error_code(666);
    else return ok;
}

typedef boost::error_info<struct tag_error_string, std::string> error_string;

bool b(bool bok, bool cok)
{
    try
    {
        if(!bok) throw ADD_EXCEPTION_INFO(error_b()) << error_string("not ok");
        else return c(cok);
    }
    catch(const error_c& e)
    {
        e << error_string("b() was called :-)");
        throw;
    }
}

typedef boost::error_info<struct tag_another_error_code, int> another_error_code;

bool a(bool aok, bool bok, bool cok)
{
    try
    {
        if(!aok) throw ADD_EXCEPTION_INFO(error_a()) << another_error_code(456);
        else return b(bok, cok);
    }
    catch(const error_c& e)
    {
        e << another_error_code(123);
        throw;
    }
    catch(const error_b& e)
    {
        e << another_error_code(123);
        throw;
    }
}

namespace utf = boost::unit_test;

bool contains_error_infos_cba(const error_c& e)
{
    BOOST_TEST((*boost::get_error_info<boost::throw_function>(e)) == "bool c(bool)");
    BOOST_TEST((*boost::get_error_info<boost::throw_file>(e)) == "test_exceptions.cpp");

    BOOST_TEST((*boost::get_error_info<error_code>(e)) == 666);
    BOOST_TEST((*boost::get_error_info<error_string>(e)) == "b() was called :-)");
    BOOST_TEST((*boost::get_error_info<another_error_code>(e)) == 123);

    BOOST_TEST_MESSAGE(boost::diagnostic_information(e));

    return true;
}

bool contains_error_infos_ba(const error_b& e)
{
    BOOST_TEST((*boost::get_error_info<boost::throw_function>(e)) == "bool b(bool, bool)");
    BOOST_TEST((*boost::get_error_info<boost::throw_file>(e)) == "test_exceptions.cpp");

    BOOST_TEST((*boost::get_error_info<error_string>(e)) == "not ok");
    BOOST_TEST((*boost::get_error_info<another_error_code>(e)) == 123);

    BOOST_TEST_MESSAGE(boost::diagnostic_information(e));

    return true;
}

bool contains_error_infos_a(const error_a& e)
{
    BOOST_TEST((*boost::get_error_info<boost::throw_function>(e)) == "bool a(bool, bool, bool)");
    BOOST_TEST((*boost::get_error_info<boost::throw_file>(e)) == "test_exceptions.cpp");

    BOOST_TEST((*boost::get_error_info<another_error_code>(e)) == 456);

    BOOST_TEST_MESSAGE(boost::diagnostic_information(e));

    return true;
}

BOOST_AUTO_TEST_SUITE(exceptions)

BOOST_AUTO_TEST_CASE(test_1)
{
    BOOST_CHECK_EXCEPTION(a(true, true, false), error_c, contains_error_infos_cba);
    BOOST_CHECK_EXCEPTION(a(true, false, true), error_b, contains_error_infos_ba );
    BOOST_CHECK_EXCEPTION(a(false, true, true), error_a, contains_error_infos_a  );
}

BOOST_AUTO_TEST_CASE(test_2)
{
    try
    {
        a(true, true, false);
    }
    catch(...)
    {
        std::cerr << "**********\n";
        std::cerr << CURRENT_LOCATION() << '\n'
                  << boost::current_exception_diagnostic_information();
        std::cerr << "**********\n";
    }
}

BOOST_AUTO_TEST_SUITE_END()
