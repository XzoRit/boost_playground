#pragma once

#include <boost/leaf.hpp>

#include <exception>
#include <utility>

namespace xzr::error
{
template <class ThrowingFunc>
constexpr inline boost::leaf::result<decltype(std::declval<ThrowingFunc>()())> try_call(ThrowingFunc&& throwing_func)
{
    using result_type = boost::leaf::result<decltype(std::declval<ThrowingFunc>()())>;

    return boost::leaf::try_catch(
        [&]() -> result_type { return throwing_func(); },
        [](const boost::leaf::error_info&) -> result_type { return boost::leaf::new_error(); });
}
} // namespace xzr::error
