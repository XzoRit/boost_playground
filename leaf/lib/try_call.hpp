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

template <class ThrowingFunc>
constexpr inline auto exception_to_result(ThrowingFunc&& throwing_func)
    -> decltype(boost::leaf::exception_to_result<>(std::forward<ThrowingFunc>(throwing_func)))
{
    return boost::leaf::exception_to_result<>(std::forward<ThrowingFunc>(throwing_func));
}
} // namespace xzr::error
