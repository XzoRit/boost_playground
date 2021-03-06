#include <read_file.hpp>

#include <boost/leaf.hpp>

#include <boost/test/unit_test.hpp>

#include <cerrno>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>

namespace leaf = boost::leaf;
namespace utf = boost::unit_test;

namespace boost::leaf
{
template <class T>
std::ostream& boost_test_print_type(std::ostream& ostr, const result<T>& r)
{
    ostr << "result -> ";
    if (r)
        ostr << "value";
    else
        ostr << "error";

    return ostr;
}
} // namespace boost::leaf

BOOST_AUTO_TEST_SUITE(boost_leaf)

BOOST_AUTO_TEST_SUITE(file_to_string)

BOOST_AUTO_TEST_SUITE(parse_command_line)

BOOST_AUTO_TEST_CASE(valid)
{
    const char* argv[] = {"first", "second"};
    leaf::result<const char*> res = xzr::error_enum::parse_command_line(2, argv);

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(res.value(), "second");
}

BOOST_AUTO_TEST_CASE(invalid)
{
    const char* err_str = "cmd_line_error ";
    const char* res = leaf::try_handle_all(
        [] { return xzr::error_enum::parse_command_line(1, nullptr); },
        [err_str](leaf::match<xzr::error_enum::error_code, xzr::error_enum::bad_command_line>) { return err_str; },
        [](const leaf::error_info& no_match) {
            BOOST_TEST_INFO("unexpected error:" << no_match);
            return nullptr;
        });

    BOOST_CHECK_EQUAL(res, err_str);
}

BOOST_AUTO_TEST_SUITE_END()

const std::filesystem::path valid_file_path{std::filesystem::temp_directory_path() / "hello.txt"};
const char* valid_file{valid_file_path.c_str()};
const int valid_file_size{7};

BOOST_AUTO_TEST_SUITE(file_open)

const char* invalid_file{""};

BOOST_AUTO_TEST_CASE(valid)
{
    const leaf::result<std::shared_ptr<FILE>> res = xzr::error_enum::file_open(valid_file);
    BOOST_REQUIRE(res);
}

BOOST_AUTO_TEST_CASE(invalid)
{
    const std::shared_ptr<FILE> res =
        leaf::try_handle_all([] { return xzr::error_enum::file_open(invalid_file); },
                             [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::open_error>,
                                leaf::match_value<leaf::e_errno, ENOENT>) { return nullptr; },
                             [](const leaf::error_info& no_match) {
                                 BOOST_TEST_INFO("unexpected error:" << no_match);
                                 return nullptr;
                             });

    BOOST_REQUIRE(!res);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(file_size, *utf::depends_on("boost_leaf/file_to_string/file_open/valid"))
{
    std::shared_ptr<FILE> f{xzr::error_enum::file_open(valid_file).value()};
    leaf::result<int> size = xzr::error_enum::file_size(*f);

    BOOST_REQUIRE(size);
    BOOST_CHECK_EQUAL(size.value(), valid_file_size);
}

BOOST_AUTO_TEST_CASE(file_read, *utf::depends_on("boost_leaf/file_to_string/file_open/valid"))
{
    const std::string zeroed_buffer(1 + valid_file_size, '\0');
    std::string buffer{zeroed_buffer};

    std::shared_ptr<FILE> f{xzr::error_enum::file_open(valid_file).value()};
    const leaf::result<void> res{xzr::error_enum::file_read(*f, buffer.data(), buffer.size() - 1)};

    BOOST_REQUIRE(res);
    BOOST_CHECK_NE(buffer, zeroed_buffer);
}

BOOST_AUTO_TEST_CASE(output_to)
{
    const std::string txt{"Hello!"};
    std::ostringstream out{};

    const leaf::result<void> res{xzr::error_enum::output_to(out, txt)};

    BOOST_REQUIRE(res);
    BOOST_CHECK_EQUAL(out.str(), txt);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
