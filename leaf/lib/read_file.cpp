#include <read_file.hpp>

#include <cerrno>

namespace leaf = boost::leaf;

namespace xzr::error_enum
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
    const auto load{leaf::on_error(input_error, [] { return leaf::e_errno{errno}; })};

    if (fseek(&f, 0, SEEK_END))
        return BOOST_LEAF_NEW_ERROR(size_error);

    int s = ftell(&f);
    if (s == -1L)
        return BOOST_LEAF_NEW_ERROR(size_error);

    if (fseek(&f, 0, SEEK_SET))
        return BOOST_LEAF_NEW_ERROR(size_error);

    return s;
} // namespace xzr::error_enum

leaf::result<void> file_read(FILE& f, void* buf, int size) noexcept
{
    const auto load{leaf::on_error(input_error)};

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
} // namespace xzr::error_enum
namespace xzr::exception
{
char const* parse_command_line(int argc, char const* argv[])
{
    if (argc == 2)
        return argv[1];
    BOOST_LEAF_THROW_EXCEPTION(bad_command_line{});
}

std::shared_ptr<FILE> file_open(char const* file_name)
{
    if (FILE* f = fopen(file_name, "rb"))
        return std::shared_ptr<FILE>(f, &fclose);
    else
        BOOST_LEAF_THROW_EXCEPTION(open_error{}, leaf::e_errno{errno});
}

int file_size(FILE& f)
{
    const auto load{leaf::on_error([] { return leaf::e_errno{errno}; })};

    if (fseek(&f, 0, SEEK_END))
        BOOST_LEAF_THROW_EXCEPTION(size_error{});

    int s = ftell(&f);
    if (s == -1L)
        BOOST_LEAF_THROW_EXCEPTION(size_error{});

    if (fseek(&f, 0, SEEK_SET))
        BOOST_LEAF_THROW_EXCEPTION(size_error{});

    return s;
}

void file_read(FILE& f, void* buf, int size)
{
    int n = fread(buf, 1, size, &f);

    if (ferror(&f))
        BOOST_LEAF_THROW_EXCEPTION(read_error{}, leaf::e_errno{errno});

    if (n != size)
        BOOST_LEAF_THROW_EXCEPTION(eof_error{});
}

void output_to(std::ostream& out, const std::string& txt)
{
    out << txt;
    out.flush();
    if (out.fail())
        BOOST_LEAF_THROW_EXCEPTION(output_error{}, leaf::e_errno{errno});
}
} // namespace xzr::exception
namespace xzr::error_type
{
leaf::result<char const*> parse_command_line(int argc, char const* argv[]) noexcept
{
    if (argc == 2)
        return argv[1];
    return BOOST_LEAF_NEW_ERROR(bad_command_line{});
}

leaf::result<std::shared_ptr<FILE>> file_open(char const* file_name) noexcept
{
    const auto load{leaf::on_error(input_error{})};

    if (FILE* f = fopen(file_name, "rb"))
        return std::shared_ptr<FILE>(f, &fclose);
    else
        return BOOST_LEAF_NEW_ERROR(open_error{}, leaf::e_errno{errno});
}

leaf::result<int> file_size(FILE& f) noexcept
{
    const auto load{leaf::on_error(input_error{}, size_error{}, [] { return leaf::e_errno{errno}; })};

    if (fseek(&f, 0, SEEK_END))
        return BOOST_LEAF_NEW_ERROR();

    int s = ftell(&f);
    if (s == -1L)
        return BOOST_LEAF_NEW_ERROR();

    if (fseek(&f, 0, SEEK_SET))
        return BOOST_LEAF_NEW_ERROR();

    return s;
}

leaf::result<void> file_read(FILE& f, void* buf, int size) noexcept
{
    const auto load{leaf::on_error(input_error{})};

    int n = fread(buf, 1, size, &f);

    if (ferror(&f))
        return BOOST_LEAF_NEW_ERROR(read_error{}, leaf::e_errno{errno});

    if (n != size)
        return BOOST_LEAF_NEW_ERROR(eof_error{});

    return {};
}

leaf::result<void> output_to(std::ostream& out, const std::string& txt) noexcept
{
    out << txt;
    out.flush();
    if (out.fail())
        return BOOST_LEAF_NEW_ERROR(output_error{}, leaf::e_errno{errno});
    return {};
}
} // namespace xzr::error_type
