#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/time_traits.hpp>
#include <chrono>

using namespace std::chrono_literals;
using namespace boost;

using boost::asio::basic_deadline_timer;
using boost::asio::io_service;
using std::chrono::seconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

namespace boost
{
namespace asio
{
template<>
struct time_traits<steady_clock>
{
    using clock_type = steady_clock;
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

    static posix_time::time_duration to_posix_duration(
        const duration_type& d)
    {
        return posix_time::microseconds(
                   duration_cast<microseconds>(d).count());
    }
};
}
}

using deadline_timer = basic_deadline_timer<steady_clock>;
using clock_type = deadline_timer::traits_type::clock_type;

const auto waiting_time{1s};

BOOST_AUTO_TEST_SUITE(boost_asio)

BOOST_AUTO_TEST_SUITE(io_service_object)

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

BOOST_AUTO_TEST_SUITE(timer)

BOOST_AUTO_TEST_CASE(std_chrono_sync_deadline_timer)
{
    io_service io;

    const auto a{clock_type::now()};
    deadline_timer t{io, waiting_time};
    t.wait();
    const auto b{clock_type::now()};

    BOOST_REQUIRE(b - a >= waiting_time);
}

BOOST_AUTO_TEST_CASE(std_chrono_async_deadline_timer)
{
    io_service io;

    deadline_timer t{io, waiting_time};
    int called{0};
    t.async_wait([&called](const auto&)
    {
        ++called;
    });

    const auto a{clock_type::now()};
    BOOST_REQUIRE(io.run() == 1);
    const auto b{clock_type::now()};

    BOOST_REQUIRE(b - a >= waiting_time);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
