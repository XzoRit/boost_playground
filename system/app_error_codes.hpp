#pragma once

#include <boost/type_traits/integral_constant.hpp>

#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>

using namespace std::string_literals;
using namespace boost::system;

namespace app
{
    class error_category_type : public error_category
    {
    public:
        const char* name() const noexcept override final;
        std::string message(int ev) const noexcept override final;
    };

    const error_category_type& error_category();
}

namespace app
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
    struct is_error_code_enum<app::error::type> : boost::true_type
    {};
}

namespace app
{
    inline const char* error_category_type::name() const noexcept
    {
        return "app";
    }

    inline std::string error_category_type::message(int ev) const noexcept
    {
        return "app error: "s + std::to_string(ev);
    }

    inline const error_category_type& error_category()
    {
        static error_category_type c;
        return c;
    }
}

namespace app
{
    inline error::error(error::type e)
        : error_code(e, error_category())
    {}

    inline error_code make_error_code(error::type ec)
    {
        return error(ec);
    }
}
