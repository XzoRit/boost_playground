#define BOOST_TEST_MODULE calculator_test
#include <boost/test/included/unit_test.hpp>
#include "calculator.hpp"

BOOST_AUTO_TEST_CASE(sum_of_comma_separated_list)
{
  BOOST_TEST(Calculator::add("1,22,333") == 356);
}
