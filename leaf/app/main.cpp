#include <read_file.hpp>

#include <boost/leaf.hpp>

#include <filesystem>
#include <iostream>
#include <string>

namespace leaf = boost::leaf;

int main(int argc, const char* argv[])
{
    return leaf::try_handle_all(
        [&]() -> leaf::result<int> {
            BOOST_LEAF_AUTO(file_name, xzr::error_code::parse_command_line(argc, argv));

            auto load = leaf::on_error(leaf::e_file_name{file_name});

            BOOST_LEAF_AUTO(f, xzr::error_code::file_open(file_name));

            BOOST_LEAF_AUTO(s, xzr::error_code::file_size(*f));

            std::string buffer(1 + s, '\0');
            BOOST_LEAF_CHECK(xzr::error_code::file_read(*f, &buffer[0], buffer.size() - 1));

            BOOST_LEAF_CHECK(xzr::error_code::output_to(std::cout, buffer));

            return 0;
        },
        [](leaf::match<xzr::error_code::error_code, xzr::error_code::open_error>,
           leaf::match_value<leaf::e_errno, ENOENT>,
           leaf::e_file_name const& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "File not found: " << fn.value << " - diagnosed at: " << loc << std::endl;
            return 1;
        },

        // This handler will be called if the error includes:
        // - an object of type error_code equal to open_error, and
        // - an object of type leaf::e_errno (regardless of its .value), and
        // - an object of type leaf::e_file_name.
        [](leaf::match<xzr::error_code::error_code, xzr::error_code::open_error>,
           leaf::e_errno const& errn,
           leaf::e_file_name const& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to open " << fn.value << ", errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 2;
        },

        // This handler will be called if the error includes:
        // - an object of type error_code equal to any of size_error, read_error, eof_error, and
        // - an optional object of type leaf::e_errno (regardless of its .value), and
        // - an object of type leaf::e_file_name.
        [](leaf::match<xzr::error_code::error_code,
                       xzr::error_code::size_error,
                       xzr::error_code::read_error,
                       xzr::error_code::eof_error>,
           leaf::e_errno const* errn,
           leaf::e_file_name const& fn,
           const leaf::e_source_location& loc) {
            std::cerr << "Failed to access " << fn.value;
            if (errn)
                std::cerr << ", errno=" << *errn;
            std::cerr << " - diagnosed at: " << loc << std::endl;
            return 3;
        },

        // This handler will be called if the error includes:
        // - an object of type error_code equal to output_error, and
        // - an object of type leaf::e_errno (regardless of its .value),
        [](leaf::match<xzr::error_code::error_code, xzr::error_code::output_error>,
           leaf::e_errno const& errn,
           const leaf::e_source_location& loc) {
            std::cerr << "Output error, errno=" << errn << " - diagnosed at: " << loc << std::endl;
            return 4;
        },

        // This handler will be called if we've got a bad_command_line
        [](leaf::match<xzr::error_code::error_code, xzr::error_code::bad_command_line>,
           const leaf::e_source_location& loc) {
            std::cout << "Bad command line argument - diagnosed at: " << loc << std::endl;
            return 5;
        },

        // This last handler matches any error: it prints diagnostic information to help debug logic errors in the
        // program, since it failed to match  an appropriate error handler to the error condition it encountered. In
        // this program this handler will never be called.
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
            return 6;
        });
}
