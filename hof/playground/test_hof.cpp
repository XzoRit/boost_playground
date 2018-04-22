#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <vector>
#include <iterator>
#include <numeric>

using namespace std;
namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

namespace
{
    int sum_free_func(int a, int b)
    {
        return a + b;
    }

    template<class T>
    T sum_template_func(T a, T b)
    {
        return a + b;
    }

    struct sum_functor
    {
        template<class T>
        T operator()(T a, T b) const
            {
                return a + b;
            }
    };
}

BOOST_AUTO_TEST_SUITE(hof)

BOOST_AUTO_TEST_CASE(sum_func)
{
    const vector<int> v = {1, 22, 333};

    auto a = accumulate(begin(v), end(v), 0, sum_free_func);
    BOOST_TEST(a == 356);

    auto sum_ints = static_cast<int(*)(int, int)>(sum_template_func);
    a = accumulate(begin(v), end(v), 0, sum_ints);
    BOOST_TEST(a == 356);

    const auto sum_obj = sum_functor{};
    a = accumulate(begin(v), end(v), 0, sum_obj);
    BOOST_TEST(a == 356);
}

BOOST_AUTO_TEST_SUITE_END()
