#include <boost/hof/lift.hpp>
#include <boost/hof/function.hpp>
#include <boost/hof/pipable.hpp>
#include <boost/hof/flow.hpp>
#include <boost/hof/infix.hpp>
#include <boost/hof/partial.hpp>
#include <boost/hof/compose.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <numeric>
#include <tuple>

#include <boost/test/unit_test.hpp>

using namespace std;

namespace fus = boost::fusion;

namespace hof = boost::hof;

namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

namespace
{
    int sum_free_func(int a, int b)
    {
        return a + b;
    }

    template<class A, class B>
    auto sum_template_func(A a, B b)
    {
        return a + b;
    }

    struct sum_functor
    {
        template<class T>
        auto operator()(T a, T b) const
            {
                return a + b;
            }
    };

    BOOST_HOF_STATIC_FUNCTION(sum) = sum_functor{};

    BOOST_HOF_STATIC_FUNCTION(sum_pipe) = hof::pipable(sum_functor{});

    BOOST_HOF_STATIC_FUNCTION(sum_infix) = hof::infix(sum_functor{});

    BOOST_HOF_STATIC_LAMBDA_FUNCTION(sum_lamb) = [](auto a, auto b) { return a + b; };

    namespace adl
    {
        using std::begin;

        template<class Range>
        auto adl_begin(Range&& r) BOOST_HOF_RETURNS(begin(r));
    }

    BOOST_HOF_STATIC_LAMBDA_FUNCTION(print) = hof::first_of(
        [](auto& str, const auto& a) -> decltype(str << a)
        {
            str << a << ' ';
            return str;
        },
        [](auto& str, const auto& range) -> decltype(str << *adl::adl_begin(range))
        {
            str << "{ ";
            for(const auto& a : range) str << a << ' ';
            str << '}';
            return str;
        },
        [](auto& str, const auto& tuple) -> decltype(auto)
        {
            str << "< ";
            fus::for_each(tuple, [&str](const auto& a){ str << a << ' '; });
            // I dont know why this does not work
            // apply([&str](const auto& a) mutable -> decltype(auto) { str << a << ' '; }, tuple);
            str << '>';
            return str;
        });
}

BOOST_AUTO_TEST_SUITE(boost_hof)

const vector<int> v = {1, 22, 333};

BOOST_AUTO_TEST_SUITE(summing)

BOOST_AUTO_TEST_CASE(sum_func)
{
    auto a = accumulate(begin(v), end(v), 0, sum_free_func);
    BOOST_TEST(a == 356);

    auto sum_ints = static_cast<int(*)(int, int)>(sum_template_func);
    a = accumulate(begin(v), end(v), 0, sum_ints);
    BOOST_TEST(a == 356);

    const auto sum_obj = sum_functor{};
    a = accumulate(begin(v), end(v), 0, sum_obj);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(lift)
{
    const auto a = accumulate(begin(v), end(v), 0, BOOST_HOF_LIFT(sum_template_func));
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(static_function)
{
    const auto a = accumulate(begin(v), end(v), 0, sum);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(pipeable_sum)
{
    const auto a = 1 | sum_pipe(22) | sum_pipe(333);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(flow_sum)
{
    const auto a = hof::flow(sum_pipe(22), sum_pipe(333))(1);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(infix_sum)
{
    const auto a = 1 <sum_infix> 22 <sum_infix> 333;
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_CASE(compose_partial_sum)
{
    const auto a = hof::partial(sum)(1);
    const auto b = hof::compose(a, a);
    const auto c = b(1);
    BOOST_TEST(c == 3);
}

BOOST_AUTO_TEST_CASE(lambda_function)
{
    const auto a = accumulate(begin(v), end(v), 0, sum_lamb);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(printing)

BOOST_AUTO_TEST_CASE(print_overloading)
{
    {
        stringstream str;

        print(str, 1);
        print(str, 22);
        print(str, 333);
        BOOST_TEST(str.str() == string("1 22 333 "));
    }
    {
        stringstream str;

        print(str, v);
        BOOST_TEST(str.str() == string("{ 1 22 333 }"));
    }
    {
        stringstream str;

        print(str, make_tuple(1, 22, 333));
        BOOST_TEST(str.str() == string("< 1 22 333 >"));
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
