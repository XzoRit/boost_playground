#define BOOST_TEST_MODULE calculator_test
#include <boost/test/included/unit_test.hpp>
#include "calculator.hpp"

BOOST_AUTO_TEST_CASE(sum_of_empty_string_is_0)
{
  BOOST_TEST(Calculator::add("") == 0);
}

BOOST_AUTO_TEST_CASE(sum_of_one_number_returns_that_number)
{
  BOOST_TEST(Calculator::add("123") == 123);
}

BOOST_AUTO_TEST_CASE(sum_of_comma_separated_list)
{
  BOOST_TEST(Calculator::add("1,22,333") == 356);
}

BOOST_AUTO_TEST_CASE(newline_is_a_separator_too)
{
  BOOST_TEST(Calculator::add("1,22\n333") == 356);
}
