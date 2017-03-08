#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>

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

MOCK_BASE_CLASS(MockView, IView)
{
    MOCK_CONST_METHOD(display, 1, void(int));
};

BOOST_AUTO_TEST_CASE(test_1)
{
    MockView v;
    Calculator c(v);

    mock::sequence s;
    MOCK_EXPECT(v.display).once().with(0).in(s);
    MOCK_EXPECT(v.display).once().with(1).in(s);

   c.add(0, 0);
   c.add(1, 0);
}
