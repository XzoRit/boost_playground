#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/test/unit_test.hpp>

#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>

using namespace boost::describe;
using namespace boost::mp11;
using namespace std::string_literals;

namespace v1
{
template <class T, class Bd = describe_bases<T, mod_any_access>, class Md = describe_members<T, mod_any_access>>
std::ostream& operator<<(std::ostream& os, T const& t)
{
    os << "{";

    bool first = true;

    mp_for_each<Bd>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        using B = typename decltype(D)::type;
        os << (B const&)t;
    });

    mp_for_each<Md>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        os << "." << D.name << " = " << t.*D.pointer;
    });

    os << "}";
    return os;
}

struct X
{
    int m1 = 1;
};

BOOST_DESCRIBE_STRUCT(X, (), (m1))

struct Y
{
    int m2 = 2;
};

BOOST_DESCRIBE_STRUCT(Y, (), (m2))

class Z : public X, private Y
{
    int m1 = 3;
    int m2 = 4;

    BOOST_DESCRIBE_CLASS(Z, (X, Y), (), (), (m1, m2))
};
}

namespace v2
{
template <class T>
struct ostreamable
{
};

template <class T, class Md = describe_members<T, mod_any_access>>
inline std::ostream& operator<<(std::ostream& os, const ostreamable<T>& o)
{
    const T& t{*static_cast<const T*>(&o)};

    os << "{ ";

    mp_for_each<Md>([&](auto D) { os << '.' << D.name << '=' << t.*D.pointer << ' '; });

    os << "}";

    return os;
}

struct X : ostreamable<X>
{
    int m1 = 1;
};

BOOST_DESCRIBE_STRUCT(X, (), (m1))

struct Y : ostreamable<Y>
{
    int m2 = 2;
};

BOOST_DESCRIBE_STRUCT(Y, (), (m2))

class Z : public ostreamable<Z>
{
    int m1 = 3;
    int m2 = 4;

    BOOST_DESCRIBE_CLASS(Z, (), (), (), (m1, m2))
};
}

namespace v3
{
namespace skill
{
template <class T, class Bd = describe_bases<T, mod_any_access>, class Md = describe_members<T, mod_any_access>>
std::ostream& operator<<(std::ostream& os, T const& t)
{
    os << "{";

    bool first = true;

    mp_for_each<Bd>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        using B = typename decltype(D)::type;
        os << (B const&)t;
    });

    mp_for_each<Md>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        os << "." << D.name << " = " << t.*D.pointer;
    });

    os << "}";
    return os;
}
}

namespace lib_a
{
using skill::operator<<;

struct X
{
    int m1 = 1;
};

BOOST_DESCRIBE_STRUCT(X, (), (m1))
}

namespace lib_b
{
using skill::operator<<;

struct Y
{
    int m2 = 2;
};

BOOST_DESCRIBE_STRUCT(Y, (), (m2))
}

namespace lib_c
{
class Z : public lib_a::X, private lib_b::Y
{
    int m1 = 3;
    int m2 = 4;

    BOOST_DESCRIBE_CLASS(Z, (lib_a::X, Y), (), (), (m1, m2))
};
}
}

namespace lib_enum
{
template <class E>
std::string to_string(E e)
{
    std::string r{};

    mp_for_each<describe_enumerators<E>>([&](auto D) mutable {
        if (e == D.value)
        {
            if (!r.empty())
                r += " or "s + D.name;
            else
                r = D.name;
        }
    });

    if (r.empty())
        return "unknown";
    return r;
}

template <class E>
E to_enum(const char* name)
{
    E e{};
    bool found{false};

    mp_for_each<describe_enumerators<E>>([&](auto D) {
        if (std::strcmp(D.name, name) == 0)
        {
            found = true;
            e = D.value;
        }
    });

    if (found)
        return e;

    throw std::invalid_argument{"invalid name: \""s + name + "\" for enum " + typeid(E).name()};
}
}

namespace lib
{
enum c_enum
{
    v1 = 1,
    v2 = 2,
    v3 = 4
};

BOOST_DESCRIBE_ENUM(c_enum, v1, v2, v3)

enum class cpp_enum
{
    v4 = 1,
    v5 = 1,
    v6 = 111
};

BOOST_DESCRIBE_ENUM(cpp_enum, v4, v5, v6)

std::ostream& operator<<(std::ostream& str, cpp_enum e)
{
    str << lib_enum::to_string(e);

    return str;
}
}

BOOST_AUTO_TEST_SUITE(boost_describe)

BOOST_AUTO_TEST_CASE(ostream_for_types_in_same_namespace)
{
    std::ostringstream str{};
    str << v1::Z{};
    BOOST_CHECK_EQUAL(str.str(), "{{.m1 = 1}, {.m2 = 2}, .m1 = 3, .m2 = 4}");
}

BOOST_AUTO_TEST_CASE(crtp_ostreamable)
{
    {
        std::ostringstream str{};
        str << v2::X{};
        BOOST_CHECK_EQUAL(str.str(), "{ .m1=1 }");
    }
    {
        std::ostringstream str{};
        str << v2::Y{};
        BOOST_CHECK_EQUAL(str.str(), "{ .m2=2 }");
    }
    {
        std::ostringstream str{};
        str << v2::Z{};
        BOOST_CHECK_EQUAL(str.str(), "{ .m1=3 .m2=4 }");
    }
}

BOOST_AUTO_TEST_CASE(ostream_with_using_declaration)
{
    {
        std::ostringstream str{};
        str << v3::lib_a::X{};
        BOOST_CHECK_EQUAL(str.str(), "{.m1 = 1}");
    }
    {
        std::ostringstream str{};
        str << v3::lib_b::Y{};
        BOOST_CHECK_EQUAL(str.str(), "{.m2 = 2}");
    }
    {
        std::ostringstream str{};
        str << v3::lib_c::Z{};
        BOOST_CHECK_EQUAL(str.str(), "{{.m1 = 1}, {.m2 = 2}, .m1 = 3, .m2 = 4}");
    }
}

BOOST_AUTO_TEST_CASE(enum_to_string)
{
    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::v1), "v1");
    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::v2), "v2");
    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::v3), "v3");

    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::cpp_enum::v4), "v4 or v5");
    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::cpp_enum::v5), "v4 or v5");
    BOOST_CHECK_EQUAL(lib_enum::to_string(lib::cpp_enum::v6), "v6");
}

BOOST_AUTO_TEST_CASE(string_to_enum)
{
    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::c_enum>("v1"), lib::v1);
    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::c_enum>("v2"), lib::v2);
    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::c_enum>("v3"), lib::v3);

    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::cpp_enum>("v4"), lib::cpp_enum::v4);
    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::cpp_enum>("v5"), lib::cpp_enum::v5);
    BOOST_CHECK_EQUAL(lib_enum::to_enum<lib::cpp_enum>("v6"), lib::cpp_enum::v6);

    BOOST_CHECK_THROW(lib_enum::to_enum<lib::c_enum>("nope"), std::invalid_argument);
    BOOST_CHECK_THROW(lib_enum::to_enum<lib::cpp_enum>("nope"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()
