#pragma once

#include <boost/type_traits/integral_constant.hpp>

#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>

using namespace std::string_literals;
using namespace boost::system;

namespace lib
{
    class error_category_type : public error_category
    {
    public:
        const char* name() const noexcept override final;
        std::string message(int ev) const noexcept override final;
    };

    const error_category_type& error_category();
}

namespace lib
{
    class error : public error_code
    {
    public:
        enum type
        {
            success = 0,
            failure
        };

        error() = default;
        explicit error(type e);
    };

    error_code make_error_code(error::type ec);

    const error_code ec_succ{make_error_code(error::success)};
    const error_code ec_fail{make_error_code(error::failure)};
}

namespace boost::system
{
    template<>
    struct is_error_code_enum<lib::error::type> : std::true_type
    {};
}

namespace lib
{
    inline const char* error_category_type::name() const noexcept
    {
        return "lib";
    }

    inline std::string error_category_type::message(int ev) const noexcept
    {
        return "lib error: "s + std::to_string(ev);
    }

    inline const error_category_type& error_category()
    {
        static error_category_type c;
        return c;
    }
}

namespace lib
{
    inline error::error(error::type e)
        : error_code(e, error_category())
    {}

    inline error_code make_error_code(error::type ec)
    {
        return error_code(static_cast<int>(ec), error_category());
    }
}
