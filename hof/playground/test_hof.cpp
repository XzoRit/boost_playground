#include <boost/hof/lift.hpp>
#include <boost/hof/function.hpp>
#include <algorithm>
#include <vector>
#include <iterator>
#include <numeric>

#include <boost/test/unit_test.hpp>

using namespace std;
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
}

BOOST_AUTO_TEST_SUITE(hof)

const vector<int> v = {1, 22, 333};

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

BOOST_AUTO_TEST_SUITE_END()
