/* needs at least gcc6 */
#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <boost/hana.hpp>

namespace hana = boost::hana;

TEST_CASE("maximum size of 2 types")
{
    hana::transform(
	hana::tuple_t<char, int>,
	[](auto a){return sizeof(a);});
}
