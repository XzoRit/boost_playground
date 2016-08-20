#include <boost/test/unit_test.hpp>
#include <vector>

struct init_vector
{
    init_vector()
    {
        BOOST_TEST_MESSAGE("init_vector");
    }
    ~init_vector()
    {
        BOOST_TEST_MESSAGE("~init_vector");
    }
    const std::size_t init_size{5};
    std::vector<int> v = std::vector<int>(init_size);
};

struct resize_vector_bigger : public init_vector
{
    resize_vector_bigger()
    {
        BOOST_TEST_MESSAGE("resize_vector_bigger");
        v.resize(bigger_size);
    }
    ~resize_vector_bigger()
    {
        BOOST_TEST_MESSAGE("~resize_vector_bigger");
    }
    const std::size_t bigger_size{init_size * 2};
};

struct resize_vector_smaller : public init_vector
{
    resize_vector_smaller()
    {
        BOOST_TEST_MESSAGE("resize_vector_smaller");
        v.resize(smaller_size);
    }
    ~resize_vector_smaller()
    {
        BOOST_TEST_MESSAGE("~resize_vector_smaller");
    }
    const std::size_t smaller_size{init_size - init_size};
};

struct reserve_vector_smaller : public init_vector
{
    reserve_vector_smaller()
    {
        BOOST_TEST_MESSAGE("resize_vector_smaller");
        v.reserve(init_size - init_size);
    }
    ~reserve_vector_smaller()
    {
        BOOST_TEST_MESSAGE("~resize_vector_smaller");
    }
};

struct reserve_vector_bigger : public init_vector
{
    reserve_vector_bigger()
        : bigger_capacity{v.capacity() * 3}
    {
        BOOST_TEST_MESSAGE("resize_vector_bigger");
        v.reserve(bigger_capacity);
    }
    ~reserve_vector_bigger()
    {
        BOOST_TEST_MESSAGE("~resize_vector_bigger");
    }
    const std::size_t bigger_capacity{0};
};

struct reserve_vector_smaller_again : public reserve_vector_bigger
{
    reserve_vector_smaller_again()
    {
        BOOST_TEST_MESSAGE("reserve_vector_smaller_again");
        v.reserve(bigger_capacity - bigger_capacity);
    }
    ~reserve_vector_smaller_again()
    {
        BOOST_TEST_MESSAGE("~reserve_vector_smaller_again");
    }
};

BOOST_AUTO_TEST_SUITE(vector)

BOOST_FIXTURE_TEST_CASE(size_and_capacity_after_init, init_vector)
{
    BOOST_TEST(v.size() == init_size);
    BOOST_TEST(v.capacity() >= init_size);
}

BOOST_FIXTURE_TEST_CASE(size_and_capacity_after_resize_bigger, resize_vector_bigger)
{
    BOOST_TEST(v.size() == bigger_size);
    BOOST_TEST(v.capacity() >= bigger_size);
}

BOOST_FIXTURE_TEST_CASE(size_and_capacity_after_resize_smaller, resize_vector_smaller)
{
    BOOST_TEST(v.size() == smaller_size);
    BOOST_TEST(v.capacity() >= init_size);
}

BOOST_FIXTURE_TEST_CASE(size_and_capacity_not_changed_after_reserve_smaller, reserve_vector_smaller)
{
    BOOST_TEST(v.size() == init_size);
    BOOST_TEST(v.capacity() >= init_size);
}

BOOST_FIXTURE_TEST_CASE(size_not_changing_but_capacity_after_reserve_bigger, reserve_vector_bigger)
{
    BOOST_TEST(v.size() == init_size);
    BOOST_TEST(v.capacity() >= bigger_capacity);
}

BOOST_FIXTURE_TEST_CASE(size_and_capacity_not_changed_after_reserve_smaller_again, reserve_vector_smaller_again)
{
    BOOST_TEST(v.size() == init_size);
    BOOST_TEST(v.capacity() >= bigger_capacity);
}

BOOST_AUTO_TEST_SUITE_END()
