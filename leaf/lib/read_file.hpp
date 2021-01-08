#pragma once

#include <boost/leaf.hpp>

#include <cstdio>
#include <memory>
#include <ostream>
#include <string>
#include <exception>
#include <stdexcept>

namespace xzr::error_enum
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
}
namespace xzr::exception
{
  struct bad_command_line : std::exception{};
  struct input_error : std::exception{};
  struct open_error : input_error{};
  struct read_error : input_error{};
  struct size_error : input_error{};
  struct eof_error : input_error{};
  struct output_error : std::exception{};

char const* parse_command_line(int argc, char const* argv[]);

std::shared_ptr<FILE> file_open(const char* file_name);

int file_size(FILE& f);

void file_read(FILE& f, void* buf, int size);

void output_to(std::ostream& out, const std::string& txt);
}
namespace xzr::error_type
{
struct bad_command_line {};
struct input_error {};
struct open_error {};
struct read_error {};
struct size_error {};
struct eof_error {};
struct output_error {};

boost::leaf::result<char const*> parse_command_line(int argc, char const* argv[]) noexcept;

boost::leaf::result<std::shared_ptr<FILE>> file_open(const char* file_name) noexcept;

boost::leaf::result<int> file_size(FILE& f) noexcept;

boost::leaf::result<void> file_read(FILE& f, void* buf, int size) noexcept;

boost::leaf::result<void> output_to(std::ostream& out, const std::string& txt) noexcept;
}
