#include <boost/leaf.hpp>

#include <cstdio>
#include <memory>
#include <ostream>
#include <string>

namespace xzr
{
enum error_code
{
    bad_command_line = 1,
    open_error,
    read_error,
    size_error,
    eof_error,
    output_error
};

boost::leaf::result<char const*> parse_command_line(int argc, char const* argv[]) noexcept;

boost::leaf::result<std::shared_ptr<FILE>> file_open(const char* file_name) noexcept;

boost::leaf::result<int> file_size(FILE& f) noexcept;

boost::leaf::result<void> file_read(FILE& f, void* buf, int size) noexcept;

boost::leaf::result<void> output_to(std::ostream& out, const std::string& txt) noexcept;
} // namespace xzr
