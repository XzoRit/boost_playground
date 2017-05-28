#include <boost/test/unit_test.hpp>
#include <turtle/mock.hpp>
#include <stdexcept>

class Result
{
public:
    explicit Result(int v)
    : value(v)
    {}

    operator int()
    {
        return value;
    }

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

   c.add(1, -1);
}

BOOST_AUTO_TEST_CASE(test_throws_exception)
{
    MockView v;
    Calculator c(v);

    MOCK_EXPECT(v.display).returns(false);

    BOOST_CHECK_THROW(c.add(1, 2), std::runtime_error);
}

bool customConstraintFunc(const Result& actual)
{
    return actual.value == 3;
}

BOOST_AUTO_TEST_CASE(test_custom_constraint_func)
{
    MockView v;
    Calculator c(v);

    MOCK_EXPECT(v.display).once().with(customConstraintFunc).returns(true);

   c.add(1, 2);
}

class CustomConstraintType
{};

bool operator==(const Result& r, const CustomConstraintType&)
{
    return r.value == 2;
}

mock::stream& operator<<(mock::stream& str, const CustomConstraintType&)
{
    str << "actual == 2";
    return str;
}

BOOST_AUTO_TEST_CASE(test_custom_constraint_type)
{
    MockView v;
    Calculator c(v);

    MOCK_EXPECT(v.display).once().with(CustomConstraintType()).returns(true);

   c.add(1, 1);
}

template<class Expected>
class NearConstraint
{
public:
    NearConstraint(Expected e, Expected t)
      : expected(e)
      , threshold(t)
    {}

    template<class Actual>
    bool operator()(Actual actual) const
    {
        return std::abs(actual - boost::unwrap_ref(expected))
            < boost::unwrap_ref(threshold);
    }

    Expected expected;
    Expected threshold;
};

template<class Expected>
mock::stream& operator<<(mock::stream& s, const NearConstraint<Expected>& c)
{
    return s << "near(" << mock::format(c.expected)
        << ", " << mock::format(c.threshold) << ")";
}

template<class Expected>
mock::constraint<NearConstraint<Expected>> near(Expected expected, Expected threshold)
{
    return NearConstraint<Expected>(expected, threshold);
}

BOOST_AUTO_TEST_CASE(test_custom_templated_stateful_constraint)
{
    MockView v;
    Calculator c(v);
    int expected;
    int threshold;

    MOCK_EXPECT(v.display)
        .once()
        .with(near(boost::cref(expected), boost::cref(threshold)))
        .returns(true);

    expected = 4;
    threshold = 1;

   c.add(1, 3);
}
