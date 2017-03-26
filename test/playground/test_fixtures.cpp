#include <boost/test/unit_test.hpp>
#include <vector>
#include <iterator>
#include <array>

using namespace std;
namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

vector<int> container;

struct SuiteFixture
{
    SuiteFixture(int element)
        {
            container.push_back(element);
        }

    ~SuiteFixture()
        {
            container.clear();
        }
};

void setup()
{
    container = vector<int>{1, 22, 333};
}

void teardown()
{
    container.clear();
}

BOOST_AUTO_TEST_SUITE(test_fixtures
                      , *utf::fixture<SuiteFixture>(2))

BOOST_AUTO_TEST_CASE(fixture_decorator_on_test_suite)
{
    BOOST_TEST(!container.empty());
    BOOST_TEST(container.size() == 1);
    BOOST_TEST(*begin(container) == 2);
}

BOOST_AUTO_TEST_CASE(fixture_decorator_on_test_case
                     , *utf::fixture(setup, teardown))
{
    BOOST_TEST(!container.empty());
    BOOST_TEST(container.size() == 3);

    array<int, 3> expected = {{1, 22, 333}};
    BOOST_TEST(expected == container, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
