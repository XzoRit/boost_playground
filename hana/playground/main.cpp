/* needs at least gcc6 */
#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <boost/hana.hpp>
#include <boost/hana/experimental/printable.hpp>

namespace hana = boost::hana;

template<class ...T>
constexpr auto sizes()
{
  return hana::transform(
           hana::tuple_t<T...>,
           [](auto a)
  {
    return hana::sizeof_(a);
  });
}

template<class ...T>
auto smallest =
  hana::fold(
    sizes<T...>(),
    [](auto a, auto b)
{
  return a < b ? a : b;
});

// template<class ...T>
// using smallest_t = decltype(smallest<T...>);

// template<class ...T>
// constexpr auto smallest_v = sizeof(smallest_t<T...>);

template<class ...T>
auto largest = hana::maximum(
                 hana::make_tuple(hana::type_c<T>...),
                 [](auto a, auto b)
{
  return hana::sizeof_(a) < hana::sizeof_(b);
});

template<class ...T>
using largest_t = typename decltype(largest<T...>)::type;

template<class ...T>
constexpr auto largest_v = sizeof(largest_t<T...>);


TEST_CASE("type with smallest size")
{
    std::cout << "\n---\n"
              << hana::experimental::print(smallest<int, char>())
              << '\n';
  // static_assert(
  //   std::is_same <
  //   smallest_t<int, long long, char, short>,
  //   char >::value, "");

    auto a = smallest<int, short, long long>();
    CHECK(a == 2);
}

TEST_CASE("type of largest size")
{
  static_assert(
    std::is_same <
    largest_t<int, long long, char, short>,
    long long >::value, "");

  static_assert(
    largest_v<int, long long, char, short> == 8,
    "");
}
