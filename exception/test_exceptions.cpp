#include <boost/test/unit_test.hpp>
#include <boost/exception/all.hpp>
#include <stdexcept>
#include <string>
#include <iostream>

class current_location
{
public:
    current_location()
        : m_func_name("")
        , m_file("")
        , m_line(0)
    {}

    current_location(const char* func_name, const char* file, int line)
        : m_func_name(func_name)
        , m_file(file)
        , m_line(line)
    {}

    const char* func() const { return m_func_name; }
    const char* file() const { return m_file     ; }
    int         line() const { return m_line     ; }
    std::string str() const
    {
        return std::string(file())
            + '(' + std::to_string(line()) + "): "
            + func();
    }
private:
    const char* m_func_name;
    const char* m_file;
    int m_line;
};

std::ostream& operator<<(std::ostream& str, const current_location& loc)
{
    str << loc.str();
    return str;
}

#define CURRENT_LOCATION() \
    current_location(BOOST_CURRENT_FUNCTION, __FILE__, __LINE__)

#if defined(EXCEPTIONS_DISABLED)

void throw_exception(
    const std::exception&,
    const current_location&);

#else

template<class E>
BOOST_NORETURN
void throw_exception(const E& e, const current_location& cur_loc)
{
    throw ::boost::enable_error_info(e)
        << ::boost::throw_function(cur_loc.func())
        << ::boost::throw_file(cur_loc.file())
        << ::boost::throw_line(cur_loc.line());
}

#endif

#define THROW_EXCEPTION_WITH_INFO(e,i)               \
    ::throw_exception(                          \
        ::boost::enable_error_info(e) << (i),   \
        CURRENT_LOCATION())

#define THROW_EXCEPTION(e)                      \
    ::throw_exception(                          \
        e,                                      \
        CURRENT_LOCATION())

class exception_base : public virtual std::exception
                     , public virtual boost::exception {};

class error_a        : public virtual exception_base {};
class error_b        : public virtual exception_base {};
class error_c        : public virtual exception_base {};

typedef boost::error_info<struct tag_error_code, int> error_code;

bool c(bool ok)
{
    if(!ok) THROW_EXCEPTION_WITH_INFO(error_c(), error_code(666));
    else return ok;
}

typedef boost::error_info<struct tag_error_string, std::string> error_string;

bool b(bool bok, bool cok)
{
    try
    {
        if(!bok) THROW_EXCEPTION_WITH_INFO(error_b(), error_string("not ok"));
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
        if(!aok) THROW_EXCEPTION_WITH_INFO(error_a(), another_error_code(456));
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

void stream_exception(std::ostream& str)
{
    try
    {
        throw;
    }
    catch(const boost::exception& e)
    {
        str << '\n' << boost::diagnostic_information(e);
    }
    catch(const std::exception& e)
    {
        str << '\n' << e.what();
    }
    catch(...)
    {
        str << "\nUnknown exception";
    }
}

template<class ParamType>
void stream_exception(std::ostream& str, ParamType param)
{
    str << param;
    stream_exception(str);
}

template<class ParamType, class... RestParamType>
void stream_exception(std::ostream& str, ParamType param, RestParamType... rest_params)
{
    str << param << ' ';
    stream_exception(str, rest_params...);
}

template<class... ParamTypes>
void stream_exception(std::ostream& str, const current_location& loc, ParamTypes... params)
{
    str << loc << ' ';
    stream_exception(str, params...);
}

bool stream_exception_and_map_to_bool(std::ostream& str, int p1, float p2, const char* p3)
{
    try
    {
        static_cast<void>(a(true, true, false));
        return true;
    }
    catch(...)
    {
        stream_exception(
            str,
            CURRENT_LOCATION(),
            "p1 =",p1,
            "p2 =",p2,
            "p3 =",p3);
    }
    return false;
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
        std::ostringstream oStr;
        oStr << "**********\n";
        oStr << CURRENT_LOCATION() << '\n'
             << boost::current_exception_diagnostic_information();
        oStr << "**********\n";
        BOOST_TEST_MESSAGE(oStr.str());
    }
}

BOOST_AUTO_TEST_CASE(test_3)
{
    std::ostringstream oStr;
    oStr << "---\n";
    BOOST_TEST(!stream_exception_and_map_to_bool(oStr, 1, 1.23, "param3"));
    oStr << "---\n";
    BOOST_TEST_MESSAGE(oStr.str());
}

BOOST_AUTO_TEST_SUITE_END()
