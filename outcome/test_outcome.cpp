#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <functional>
#include <memory>

namespace clib
{
    using malloc_func_t = std::function<void*(std::size_t)>;
    malloc_func_t malloc_func = std::malloc;

    const int valid_file_descriptor   =  0;
    const int invalid_file_descriptor = -1;

    using open_func_t = std::function<int(const char*)>;
    const auto ret_valid_file_descriptor = [](auto){ return valid_file_descriptor; };
    open_func_t open_func = ret_valid_file_descriptor;

    int open(const char* path)
    {
        return open_func(path);
    }

    void* malloc(std::size_t size)
    {
        return malloc_func(size);
    }

    void free(int)
    {
        // stub
    }

    void init()
    {
        malloc_func = std::malloc;
        open_func   = ret_valid_file_descriptor;
    }
}

namespace file_utils
{
    struct handle
    {
        explicit handle(int fd)
            : file_descriptor(fd)
            {}
        ~handle()
            {
                if (file_descriptor != -1) clib::free(file_descriptor);
            }
        int file_descriptor;
    };

    namespace v1
    {
        struct error
        {
            enum {none = 0, no_mem, not_found};
        };

        int openfile(handle** out, const char* path)
        {
            *out = (handle*)clib::malloc(sizeof(struct handle));
            if(!*out) return error::no_mem;

            (*out)->file_descriptor = clib::open(path);
            if((*out)->file_descriptor == -1)
            {
                std::free(*out);
                *out = nullptr;
                return error::not_found;
            }

            return error::none;
        }
    }
    namespace v2
    {
        std::unique_ptr<handle> openfile(const char* path)
        {
            int file_descriptor = clib::open(path);
            if(file_descriptor != clib::valid_file_descriptor)
            {
                throw std::runtime_error("file not found");
            }

            return std::make_unique<handle>(file_descriptor);
        }
    }
}

struct fixture
{
     fixture() { clib::init(); }
    ~fixture() { clib::init(); }
    file_utils::handle* file;
};

namespace utf = boost::unit_test;

BOOST_AUTO_TEST_SUITE(file)

BOOST_AUTO_TEST_SUITE(v1)

using namespace file_utils;
using namespace file_utils::v1;

BOOST_FIXTURE_TEST_CASE(no_error, fixture)
{
    BOOST_CHECK_EQUAL(openfile(&file, "./foo.txt"), error::none);
}

BOOST_FIXTURE_TEST_CASE(no_memory, fixture)
{
    handle* file;

    clib::malloc_func = [](auto){ return nullptr; };

    BOOST_CHECK_EQUAL(openfile(&file, "./foo.txt"), error::no_mem);
}

BOOST_AUTO_TEST_CASE(not_found)
{
    handle* file;

    clib::open_func = [](auto){ return clib::invalid_file_descriptor; };

    BOOST_CHECK_EQUAL(openfile(&file, "./foo.txt"), error::not_found);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
