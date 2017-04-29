#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>

BOOST_AUTO_TEST_SUITE(asio)

BOOST_AUTO_TEST_SUITE(io_service)

using boost::asio::io_service;

BOOST_AUTO_TEST_CASE(poll_one_on_empty_io_service)
{
    io_service io;

    BOOST_REQUIRE(!io.stopped());
    BOOST_REQUIRE(io.poll_one() == 0);
    BOOST_REQUIRE(io.stopped());

    io.dispatch([]() {});
    BOOST_REQUIRE(io.stopped());
    BOOST_REQUIRE(io.poll_one() == 0);
}

BOOST_AUTO_TEST_CASE(poll_one_on_io_service_with_one_handler)
{
    io_service io;
    int called{0};
    io.dispatch([&called]()
    {
        ++called;
    });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(io.stopped());
    BOOST_REQUIRE(called == 1);
}

BOOST_AUTO_TEST_CASE(poll_one_on_io_service_with_many_handlers)
{
    io_service io;
    int called{0};
    io.dispatch([&called]()
    {
        ++called;
    });
    io.dispatch([&called]()
    {
        ++called;
    });
    io.dispatch([&called]()
    {
        ++called;
    });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 2);

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 3);

    BOOST_REQUIRE(io.stopped());
}

BOOST_AUTO_TEST_CASE(reset_io_service_to_call_handlers_after_poll_one)
{
    io_service io;
    int called{0};
    io.dispatch([&called]()
    {
        ++called;
    });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);
    BOOST_REQUIRE(io.stopped());

    io.reset();

    BOOST_REQUIRE(!io.stopped());

    io.dispatch([&called]()
    {
        ++called;
    });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(io.stopped());
    BOOST_REQUIRE(called == 2);
}

BOOST_AUTO_TEST_CASE(wrap_work_for_being_dispatched_later)
{
    io_service io;

    int called{0};
    io.dispatch([&called]()
    {
        ++called;
    });
    auto wrapped{io.wrap([&called]()
    {
        ++called;
    })};
    wrapped();

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

#include <boost/asio/time_traits.hpp>
#include <chrono>

namespace boost
{
namespace asio
{
template<>
struct time_traits<std::chrono::steady_clock>
{
    using clock_type = std::chrono::steady_clock;
    using time_type = clock_type::time_point;
    using duration_type = clock_type::duration;

    static time_type now()
    {
        return clock_type::now();
    }

    static time_type add(const time_type& t, const duration_type& d)
    {
        return t + d;
    }

    static duration_type subtract(const time_type& t1, const time_type& t2)
    {
        return t1 - t2;
    }

    static bool less_than(const time_type& t1, const time_type& t2)
    {
        return t1 < t2;
    }

    static boost::posix_time::time_duration to_posix_duration(
        const duration_type& d)
    {
        return boost::posix_time::microseconds(
                   std::chrono::duration_cast<std::chrono::microseconds>(d).count());
    }
};
}
}

#include <boost/asio/basic_deadline_timer.hpp>

using boost::asio::basic_deadline_timer;

using deadline_timer = basic_deadline_timer<std::chrono::steady_clock>;

using boost::asio::io_service;

BOOST_AUTO_TEST_CASE(_1_)
{
    io_service io;

    deadline_timer t{io, std::chrono::seconds{5}};

    t.wait();
}
