#define BOOST_TEST_MODULE mock_playground
#include <boost/test/included/unit_test.hpp>
#include <turtle/mock.hpp>

struct base_class
{
    virtual ~base_class()
    {}
    virtual void method(int) = 0;
};

MOCK_BASE_CLASS(mock_class, base_class)
{
    MOCK_METHOD(method, 1, void(int), id_1)
};

class IView
{
public:
    virtual void display(int) const = 0;
};

class Calculator
{
public:
    Calculator(const IView& view)
        : m_view(view)
    {}

    void add(int a, int b)
    {
        m_view.display(a + b);
    }
private:
    const IView& m_view;
};

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(test_suite_1)

BOOST_AUTO_TEST_CASE(test_1)
{
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
