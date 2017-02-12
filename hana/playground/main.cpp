/* needs at least gcc6 */
#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <boost/hana.hpp>

namespace hana = boost::hana;

template<class ...T>
auto smallest = hana::minimum(
                  hana::make_tuple(hana::type_c<T>...),
                  [](auto a, auto b)
{
  return hana::sizeof_(a) < hana::sizeof_(b);
});

template<class ...T>
using smallest_t = typename decltype(smallest<T...>)::type;


template<class ...T>
auto largest = hana::maximum(
    hana::make_tuple(hana::type_c<T>...),
    [](auto a, auto b)
{
  return hana::sizeof_(a) < hana::sizeof_(b);
});

template<class ...T>
using largest_t = typename decltype(largest<T...>)::type;


TEST_CASE("type with smallest size")
{
  static_assert(
    std::is_same <
    smallest_t<int, long long, char, short>,
    char >::value, "");
}

TEST_CASE("type of largest size")
{
  static_assert(
    std::is_same <
    largest_t<int, long long, char, short>,
    long long >::value, "");
}
