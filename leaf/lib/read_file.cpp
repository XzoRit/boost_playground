#include <read_file.hpp>

#include <cerrno>

namespace leaf = boost::leaf;

namespace xzr::error_code
{
leaf::result<char const*> parse_command_line(int argc, char const* argv[]) noexcept
{
    if (argc == 2)
        return argv[1];
    return BOOST_LEAF_NEW_ERROR(bad_command_line);
}

leaf::result<std::shared_ptr<FILE>> file_open(char const* file_name) noexcept
{
    if (FILE* f = fopen(file_name, "rb"))
        return std::shared_ptr<FILE>(f, &fclose);
    else
        return BOOST_LEAF_NEW_ERROR(open_error, leaf::e_errno{errno});
}

leaf::result<int> file_size(FILE& f) noexcept
{
    if (fseek(&f, 0, SEEK_END))
        return BOOST_LEAF_NEW_ERROR(size_error);

    int s = ftell(&f);
    if (s == -1L)
        return BOOST_LEAF_NEW_ERROR(size_error);

    if (fseek(&f, 0, SEEK_SET))
        return BOOST_LEAF_NEW_ERROR(size_error);

    return s;
}

leaf::result<void> file_read(FILE& f, void* buf, int size) noexcept
{
    int n = fread(buf, 1, size, &f);

    if (ferror(&f))
        return BOOST_LEAF_NEW_ERROR(read_error, leaf::e_errno{errno});

    if (n != size)
        return BOOST_LEAF_NEW_ERROR(eof_error);

    return {};
}

leaf::result<void> output_to(std::ostream& out, const std::string& txt) noexcept
{
    out << txt;
    out.flush();
    if (out.fail())
        return BOOST_LEAF_NEW_ERROR(output_error, leaf::e_errno{errno});
    return {};
}
} // namespace xzr::error_code
