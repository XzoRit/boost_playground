#include <boost/test/unit_test.hpp>

using namespace std;
namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

template<class ... Ts>
class restorer
{
public:
    restorer(Ts& ... ts)
        : values{tie(ts...)}
        , refs  {tie(ts...)}
    {
    }
    ~restorer()
    {
        try { refs = values; }
        catch(...) {}
    }
    tuple<Ts... > values;
    tuple<Ts&...> refs  ;
};

template<class ... Ts>
restorer<Ts...> make_restorer(Ts& ... ts)
{
    return restorer<Ts...>{ts...};
}

BOOST_AUTO_TEST_SUITE(restorer_suite)

BOOST_AUTO_TEST_CASE(ctor_takes_one_parameter)
{
    int i{1};
    {
        const auto& _{make_restorer(i)};
        (void)_;
        i = 2;
    }
    BOOST_TEST(i == 1);
}

BOOST_AUTO_TEST_CASE(ctor_takes_many_parameters)
{
    using ints            = vector<int        >;
    using strings         = list  <string     >;
    using ints_to_strings = map   <int, string>;
    auto is       = ints           {};
    auto ss       = strings        {};
    auto is_to_ss = ints_to_strings{};
    {
        const auto& _{make_restorer(is, ss, is_to_ss)};
        (void)_;
        is.push_back(123);
        ss.push_back("123");
        is_to_ss.insert({123, "123"});
    }
    BOOST_TEST(is       == ints           {});
    BOOST_TEST(ss       == strings        {});
    BOOST_TEST(is_to_ss == ints_to_strings{});
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(use_restorer)

BOOST_AUTO_TEST_CASE(restorer_in_use)
{
    vector<int> v( 5 );

    BOOST_TEST(v.size() == 5);
    BOOST_TEST(v.capacity() >= 5);

    {
        auto _{make_restorer(v)};

        v.resize(10);

        BOOST_TEST( v.size() == 10 );
        BOOST_TEST( v.capacity() >= 10 );
    }
    {
        auto _{make_restorer(v)};
        v.resize( 0 );

        BOOST_TEST( v.size() == 0 );
        BOOST_TEST( v.capacity() >= 5 );
    }
    {
        auto _{make_restorer(v)};
        v.reserve( 10 );

        BOOST_TEST( v.size() == 5 );
        BOOST_TEST( v.capacity() >= 10 );
        {
            auto _{make_restorer(v)};
            v.reserve( 7 );

            BOOST_TEST( v.capacity() >= 10 );
        }
    }
    {
        auto _{make_restorer(v)};
        v.reserve( 0 );

        BOOST_TEST( v.size() == 5 );
        BOOST_TEST( v.capacity() >= 5 );
    }
}

BOOST_AUTO_TEST_SUITE_END()
