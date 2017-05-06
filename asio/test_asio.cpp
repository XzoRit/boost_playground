#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/asio/basic_deadline_timer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/time_traits.hpp>
#include <chrono>
#include <thread>
#include <iostream>

namespace std
{
    namespace chrono
    {
        template<class rep, class per>
        ostream& operator<<(ostream& str, const duration<rep, per>& d)
        {
            str << d.count();
            return str;
        }
    }
}

using namespace std;
using namespace std::chrono_literals;
using namespace boost;

namespace boost_system = boost::system;

using boost::asio::error::operation_aborted;
using boost::asio::io_service;

using timeout_timer = boost::asio::steady_timer;
using clock_type = timeout_timer::clock_type;

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

BOOST_AUTO_TEST_CASE(std_chrono_sync_timeout_timer)
{
    io_service io;
    timeout_timer t{io};

    const auto a{clock_type::now()};
    t.expires_from_now(waiting_time);
    t.wait();
    const auto b{clock_type::now()};

    BOOST_REQUIRE(b - a >= waiting_time);
}

BOOST_AUTO_TEST_CASE(std_chrono_async_timeout_timer)
{
    io_service io;
    timeout_timer t{io};
    int called{0};

    BOOST_REQUIRE(t.expires_from_now(waiting_time) == 0);
    t.async_wait([&called](const auto& e)
    {
        if (e != boost::asio::error::operation_aborted) ++called;
    });
    t.async_wait([&called](const auto& e)
    {
        if (e != boost::asio::error::operation_aborted) ++called;
    });
    t.async_wait([&called](const auto& e)
    {
        if (e != boost::asio::error::operation_aborted) ++called;
    });

    BOOST_REQUIRE(io.run() == 3);
    BOOST_REQUIRE(called == 3);
}

BOOST_AUTO_TEST_CASE(cancel_async_timeout_timer)
{
    io_service io;
    boost_system::error_code error_code{};

    timeout_timer t{io, waiting_time};
    t.async_wait([&error_code](const auto& ec)
    {
        error_code = ec;
    });
    t.async_wait([&error_code](const auto& ec)
    {
        error_code = ec;
    });
    t.async_wait([&error_code](const auto& ec)
    {
        error_code = ec;
    });
    t.cancel();

    BOOST_REQUIRE(io.run() == 3);
    BOOST_REQUIRE(error_code == operation_aborted);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
