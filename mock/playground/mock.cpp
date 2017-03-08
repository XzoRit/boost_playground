#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>
#include <stdexcept>

class Result
{
public:
    explicit Result(int v)
    : value(v)
    {}

    const int value;
};

bool operator==(const Result& lhs, const Result& rhs)
{
    return lhs.value == rhs.value;
}

bool operator!=(const Result& lhs, const Result& rhs)
{
    return !(lhs == rhs);
}

mock::stream& operator<<(mock::stream& str, const Result& r)
{
    str << "Result(" << r.value << ')';
    return str;
}

class IView
{
public:
    virtual bool display(const Result&) const = 0;
};

class Calculator
{
public:
    Calculator(const IView& view)
        : m_view(view)
    {}

    void add(int a, int b)
    {
        if(!m_view.display(Result(a + b)))
        {
            throw std::runtime_error("could not display result");
        }
    }
private:
    const IView& m_view;
};

MOCK_BASE_CLASS(MockView, IView)
{
    MOCK_CONST_METHOD(display, 1, bool(const Result&));
};

BOOST_AUTO_TEST_CASE(test_success)
{
    MockView v;
    Calculator c(v);

    mock::sequence s1, s2;
    MOCK_EXPECT(v.display).once().with(Result(0)).in(s1)    .returns(true);
    MOCK_EXPECT(v.display).once().with(Result(1)).in(s2)    .returns(true);
    MOCK_EXPECT(v.display)       .with(Result(2)).in(s1, s2).returns(true);

   c.add(0, 0);
   c.add(1, 0);
   c.add(1, 1);
   c.add(0, 2);
}

BOOST_AUTO_TEST_CASE(test_stream_result_via_mock_stream)
{
    MockView v;
    Calculator c(v);

    MOCK_EXPECT(v.display).once().with(Result(0)).returns(true);

   c.add(1, 2);
}
