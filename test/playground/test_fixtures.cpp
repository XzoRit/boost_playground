#include <boost/test/unit_test.hpp>
#include <vector>
#include <iterator>
#include <array>
#include <iostream>

using namespace std;
namespace utf = boost::unit_test;
namespace tt  = boost::test_tools;

vector<int> container;

struct SuiteFixture
{
    SuiteFixture(int element)
    {
        std::cout << "SuiteFixture::ctor" << element << "\n";
        container.push_back(element);
    }

    ~SuiteFixture()
    {
        std::cout << "SuiteFixture::dtor\n";
    }
};

void setup()
{
    container.push_back(22);
    container.push_back(333);
}

void teardown()
{
    container.clear();
}

BOOST_AUTO_TEST_SUITE(test_fixtures
                      , *utf::fixture<SuiteFixture>(1))

BOOST_AUTO_TEST_CASE(fixture_decorator_on_test_suite)
{
    BOOST_TEST(!container.empty());
    BOOST_TEST(container.size() == 1);
    BOOST_TEST(*begin(container) == 1);
}

BOOST_AUTO_TEST_CASE(fixture_decorator_on_test_case
                     , *utf::fixture(setup, teardown))
{
    BOOST_TEST(!container.empty());
    BOOST_TEST(container.size() == 3);

    array<int, 3> expected = {{1, 22, 333}};
    BOOST_TEST(expected == container, tt::per_element());
}

BOOST_AUTO_TEST_CASE(lambda_setup_teardown
                     , *utf::fixture(
                         [] { container.push_back(4444) ; },
                         [] { container.push_back(55555);}))
{
    BOOST_TEST(!container.empty());
    BOOST_TEST(container.size() == 1);
    BOOST_TEST(*begin(container) == 4444);
}

BOOST_AUTO_TEST_CASE(lambda_teardown_check)
{
    BOOST_TEST(container.size() == 2);

    array<int, 2> expected = {{4444, 55555}};

    BOOST_TEST(expected == container, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()

template<class... T>
class restorer
{
public:
    restorer(T... ts)
        : m_values{std::make_tuple(ts...)}
    {
    }
private:
    std::tuple<T...> m_values;
};

BOOST_AUTO_TEST_SUITE(restorer_suite)

BOOST_AUTO_TEST_CASE(ctor_takes_one_parameter)
{
    using ints = std::vector<int>;
    auto is = ints{};
    auto r = restorer<ints>{is};
}

BOOST_AUTO_TEST_CASE(ctor_takes_many_parameters)
{
    using ints            = std::vector<int             >;
    using strings         = std::list  <std::string     >;
    using ints_to_strings = std::map   <int, std::string>;
    auto is       = ints           {};
    auto ss       = strings        {};
    auto is_to_ss = ints_to_strings{};
    auto r = restorer<ints, strings, ints_to_strings>{is, ss, is_to_ss};
}

BOOST_AUTO_TEST_SUITE_END()
