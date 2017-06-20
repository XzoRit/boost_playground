#include <boost/test/unit_test.hpp>
#include <cstdlib>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <system_error>

using namespace std::string_literals;

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
                *out = NULL;
                return error::not_found;
            }

            return error::none;
        }
    }
    namespace v2
    {
        handle* openfile(const char* path)
        {
            int file_descriptor = clib::open(path);

            if(file_descriptor != clib::valid_file_descriptor) return NULL;

            return new handle(file_descriptor);
        }
    }
    namespace v3
    {
        std::unique_ptr<handle> openfile(const char* path) noexcept
        {
            int file_descriptor = clib::open(path);

            if(file_descriptor != clib::valid_file_descriptor) return{};

            try { return std::make_unique<handle>(file_descriptor); }
            catch(...) {}

            return {};
        }
    }
    namespace v4
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
    namespace v5
    {
        std::unique_ptr<handle> openfile(std::error_code& ec, const char* path)
        {
            ec.clear();
            int file_descriptor = clib::open(path);
            if(file_descriptor != clib::valid_file_descriptor)
            {
                ec = std::make_error_code(std::errc::bad_file_descriptor);
                return {};
            }

            auto* a = new(std::nothrow) handle(file_descriptor);
            if(!a)
            {
                clib::free(file_descriptor);
            }

            return std::unique_ptr<handle>{a};
        }
    }
}

struct fixture
{
     fixture() { clib::init(); }
    ~fixture() { clib::init(); }

    file_utils::handle* file;
    std::error_code ec;
};

namespace utf = boost::unit_test;

const auto ret_invalid_file_descriptor =
    [](auto){ return clib::invalid_file_descriptor; };

const auto check_file_not_found =
    [](const auto& e) { return e.what() == "file not found"s; };

BOOST_AUTO_TEST_SUITE(file)

using namespace file_utils;

BOOST_AUTO_TEST_SUITE(v1)

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

BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
    clib::open_func = ret_invalid_file_descriptor;

    BOOST_CHECK_EQUAL(openfile(&file, "./foo.txt"), error::not_found);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(v2)

using namespace file_utils::v2;

BOOST_FIXTURE_TEST_CASE(no_error, fixture)
{
    if (handle* f = openfile("./foo.txt"))
    {
        delete f;
    }
    else BOOST_TEST_FAIL("handle shall be created");
}

BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
    clib::open_func = ret_invalid_file_descriptor;

    if (handle* f = openfile("./foo.txt"))
    {
        delete f;
        BOOST_TEST_FAIL("handle shall not be created");
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(v3)

using namespace file_utils::v3;

BOOST_FIXTURE_TEST_CASE(no_error, fixture)
{
    if (std::unique_ptr<handle> f = openfile("./foo.txt"))
    {
        (void)f;
    }
    else BOOST_TEST_FAIL("handle shall be created");
}

BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
    clib::open_func = ret_invalid_file_descriptor;

    if (std::unique_ptr<handle> f = openfile("./foo.txt"))
    {
        (void)f;
        BOOST_TEST_FAIL("handle shall not be created");
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(v4)

using namespace file_utils::v4;

BOOST_FIXTURE_TEST_CASE(no_error, fixture)
{
    if (std::unique_ptr<handle> f = openfile("./foo.txt"))
    {
        (void)f;
    }
    else BOOST_TEST_FAIL("handle shall be created");
}

BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
    clib::open_func = ret_invalid_file_descriptor;

    BOOST_CHECK_EXCEPTION(
        openfile("./foo.txt"),
        std::runtime_error,
        check_file_not_found);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(v5)

using namespace file_utils::v5;

BOOST_FIXTURE_TEST_CASE(no_error, fixture)
{
    std::unique_ptr<handle> f = openfile(ec, "./foo.txt");

    BOOST_TEST(!ec);

    (void)f;
}

BOOST_FIXTURE_TEST_CASE(not_found, fixture)
{
    clib::open_func = ret_invalid_file_descriptor;

    std::unique_ptr<handle> f = openfile(ec, "./foo.txt");

    BOOST_TEST(ec);

    (void)f;
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
