#include <read_file.hpp>

#include <boost/leaf.hpp>

#include <filesystem>
#include <iostream>
#include <string>
#include <tuple>

namespace leaf = boost::leaf;

int main(int argc, const char* argv[])
{
    auto unmatched_error_handlers{std::make_tuple(
        [](const leaf::error_info& unmatched, const leaf::e_source_location& loc) {
            std::cerr << "Unknown failure detected" << std::endl
                      << "Cryptic diagnostic information follows" << std::endl
                      << unmatched << " - diagnosed at: " << loc;
            return 6;
        },
        [](const leaf::error_info& unmatched) {
            std::cerr << "Unknown failure detected" << std::endl
                      << "Cryptic diagnostic information follows" << std::endl
                      << unmatched;
            return 7;
        })};

    int app_ret_val_1{leaf::try_handle_all(
        [&]() -> leaf::result<int> {
            BOOST_LEAF_AUTO(file_name, xzr::error_enum::parse_command_line(argc, argv));

            const auto load{leaf::on_error(leaf::e_file_name{file_name})};

            BOOST_LEAF_AUTO(f, xzr::error_enum::file_open(file_name));

            BOOST_LEAF_AUTO(s, xzr::error_enum::file_size(*f));

            std::string buffer(1 + s, '\0');
            BOOST_LEAF_CHECK(xzr::error_enum::file_read(*f, &buffer[0], buffer.size() - 1));

            BOOST_LEAF_CHECK(xzr::error_enum::output_to(std::cout, buffer));

            return 0;
        },
        [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::open_error>,
           leaf::match_value<leaf::e_errno, ENOENT>,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "File not found: " << fn.value << " - diagnosed at: " << loc << std::endl;
            return 1;
        },
        [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::open_error>,
           const leaf::e_errno& errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to open " << fn.value << ", errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 2;
        },
        [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::input_error>,
           const leaf::e_errno* errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to access " << fn.value;
            if (errn)
                std::cerr << ", errno=" << *errn;
            std::cerr << " - diagnosed at: " << loc << std::endl;
            return 3;
        },
        [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::output_error>,
           const leaf::e_errno& errn,
           const leaf::e_source_location& loc) {
            std::cerr << "Output error, errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 4;
        },
        [](leaf::match<xzr::error_enum::error_code, xzr::error_enum::bad_command_line>,
           const leaf::e_source_location& loc) {
            std::cerr << "Bad command line argument - diagnosed at: " << loc << std::endl;
            return 5;
        },
        unmatched_error_handlers)};

    int app_ret_val_2{leaf::try_catch(
        [&] {
            const char* file_name{xzr::exception::parse_command_line(argc, argv)};

            const auto load{leaf::on_error(leaf::e_file_name{file_name})};

            const auto f{xzr::exception::file_open(file_name)};

            const auto s{xzr::exception::file_size(*f)};

            std::string buffer(1 + s, '\0');
            xzr::exception::file_read(*f, &buffer[0], buffer.size() - 1);

            xzr::exception::output_to(std::cout, buffer);

            return 0;
        },
        [](const xzr::exception::open_error&,
           leaf::match_value<leaf::e_errno, ENOENT>,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "File not found: " << fn.value << " - diagnosed at: " << loc << std::endl;
            return 1;
        },
        [](const xzr::exception::open_error&,
           const leaf::e_errno& errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to open " << fn.value << ", errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 2;
        },
        [](const xzr::exception::input_error&,
           const leaf::e_errno* errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to access " << fn.value;
            if (errn)
                std::cerr << ", errno=" << *errn;
            std::cerr << " - diagnosed at: " << loc << std::endl;
            return 3;
        },
        [](const xzr::exception::output_error&, const leaf::e_errno& errn, const leaf::e_source_location& loc) {
            std::cerr << "Output error, errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 4;
        },
        [](const xzr::exception::bad_command_line&, const leaf::e_source_location& loc) {
            std::cerr << "Bad command line argument - diagnosed at: " << loc << std::endl;
            return 5;
        },
        unmatched_error_handlers)};

    int app_ret_val_3{leaf::try_handle_all(
        [&]() -> leaf::result<int> {
            BOOST_LEAF_AUTO(file_name, xzr::error_type::parse_command_line(argc, argv));

            const auto load{leaf::on_error(leaf::e_file_name{file_name})};

            BOOST_LEAF_AUTO(f, xzr::error_type::file_open(file_name));

            BOOST_LEAF_AUTO(s, xzr::error_type::file_size(*f));

            std::string buffer(1 + s, '\0');
            BOOST_LEAF_CHECK(xzr::error_type::file_read(*f, &buffer[0], buffer.size() - 1));

            BOOST_LEAF_CHECK(xzr::error_type::output_to(std::cout, buffer));

            return 0;
        },
        [](xzr::error_type::open_error,
           leaf::match_value<leaf::e_errno, ENOENT>,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "File not found: " << fn.value << " - diagnosed at: " << loc << std::endl;
            return 1;
        },
        [](xzr::error_type::open_error,
           const leaf::e_errno& errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to open " << fn.value << ", errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 2;
        },
        [](xzr::error_type::input_error,
           const leaf::e_errno* errn,
           const leaf::e_file_name& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to access " << fn.value;
            if (errn)
                std::cerr << ", errno=" << *errn;
            std::cerr << " - diagnosed at: " << loc << std::endl;
            return 3;
        },
        [](xzr::error_type::output_error, const leaf::e_errno& errn, const leaf::e_source_location& loc) {
            std::cerr << "Output error, errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 4;
        },
        [](xzr::error_type::bad_command_line, const leaf::e_source_location& loc) {
            std::cerr << "Bad command line argument - diagnosed at: " << loc << std::endl;
            return 5;
        },
        unmatched_error_handlers)};

    return app_ret_val_1 + app_ret_val_2 + app_ret_val_3;
}
