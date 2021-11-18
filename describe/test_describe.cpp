#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/test/unit_test.hpp>

#include <ostream>
#include <sstream>

using namespace boost::describe;

namespace v1
{
template <class T, class Bd = describe_bases<T, mod_any_access>, class Md = describe_members<T, mod_any_access>>
std::ostream& operator<<(std::ostream& os, T const& t)
{
    os << "{";

    bool first = true;

    boost::mp11::mp_for_each<Bd>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        using B = typename decltype(D)::type;
        os << (B const&)t;
    });

    boost::mp11::mp_for_each<Md>([&](auto D) {
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
} // namespace v1

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

    boost::mp11::mp_for_each<Md>([&](auto D) { os << '.' << D.name << '=' << t.*D.pointer << ' '; });

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
} // namespace v2

namespace v3
{
namespace skill
{
template <class T, class Bd = describe_bases<T, mod_any_access>, class Md = describe_members<T, mod_any_access>>
std::ostream& operator<<(std::ostream& os, T const& t)
{
    os << "{";

    bool first = true;

    boost::mp11::mp_for_each<Bd>([&](auto D) {
        if (!first)
        {
            os << ", ";
        }
        first = false;

        using B = typename decltype(D)::type;
        os << (B const&)t;
    });

    boost::mp11::mp_for_each<Md>([&](auto D) {
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
} // namespace skill

namespace lib_a
{
using skill::operator<<;

struct X
{
    int m1 = 1;
};

BOOST_DESCRIBE_STRUCT(X, (), (m1))
} // namespace lib_a

namespace lib_b
{
using skill::operator<<;

struct Y
{
    int m2 = 2;
};

BOOST_DESCRIBE_STRUCT(Y, (), (m2))
} // namespace lib_b

namespace lib_c
{
class Z : public lib_a::X, private lib_b::Y
{
    int m1 = 3;
    int m2 = 4;

    BOOST_DESCRIBE_CLASS(Z, (lib_a::X, Y), (), (), (m1, m2))
};
} // namespace lib_c
} // namespace v3

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

BOOST_AUTO_TEST_SUITE_END()
