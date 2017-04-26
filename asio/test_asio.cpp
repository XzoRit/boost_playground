#include <boost/test/unit_test.hpp>
#include <boost/asio.hpp>

BOOST_AUTO_TEST_SUITE(asio)

BOOST_AUTO_TEST_SUITE(io_service)

BOOST_AUTO_TEST_CASE(poll_one_on_empty_io_service)
{
    boost::asio::io_service io;

    BOOST_REQUIRE(io.poll_one() == 0);

    io.dispatch([](){});
    BOOST_REQUIRE(io.poll_one() == 0);
}

BOOST_AUTO_TEST_CASE(poll_one_on_io_service_with_one_handler)
{
    boost::asio::io_service io;
    int called{0};
    io.dispatch([&called](){ ++called; });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);

    io.dispatch([&called](){ ++called; });

    BOOST_REQUIRE(io.poll_one() == 0);
    BOOST_REQUIRE(called == 1);
}

BOOST_AUTO_TEST_CASE(poll_one_on_io_service_with_many_handlers)
{
    boost::asio::io_service io;
    int called{0};
    io.dispatch([&called](){ ++called; });
    io.dispatch([&called](){ ++called; });
    io.dispatch([&called](){ ++called; });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 2);

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 3);
}

BOOST_AUTO_TEST_CASE(reset_io_service_to_call_handlers_after_poll_one)
{
    boost::asio::io_service io;
    int called{0};
    io.dispatch([&called](){ ++called; });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 1);

    io.reset();
    io.dispatch([&called](){ ++called; });

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 2);

    io.dispatch([&called](){ ++called; });
    io.reset();

    BOOST_REQUIRE(io.poll_one() == 1);
    BOOST_REQUIRE(called == 3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
