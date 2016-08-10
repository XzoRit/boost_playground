/* ../../../boost/b2 -sBOOST_ROOT=../../../boost && ../bin/test_playground --log_level=message */
#define BOOST_TEST_MODULE test_playground
#include <boost/test/included/unit_test.hpp>
#include <vector>

struct init_vector
{
    init_vector() {}
    ~init_vector() {}
    const std::size_t init_size{5};
    std::vector<int> v = std::vector<int>(init_size);
};

struct resize_vector_bigger : public init_vector
{
    resize_vector_bigger()
	{
	    v.resize(bigger_size);
	}
    ~resize_vector_bigger() {}
    const std::size_t bigger_size{init_size * 2};
};

struct resize_vector_smaller : public init_vector
{
    resize_vector_smaller()
	{
	    v.resize(smaller_size);
	}
    ~resize_vector_smaller() {}
    const std::size_t smaller_size{init_size - init_size};
};

BOOST_FIXTURE_TEST_SUITE(vector, init_vector)

    BOOST_AUTO_TEST_CASE(size_and_capacity_after_init)
    {
        BOOST_TEST(v.size() == init_size);
        BOOST_TEST(v.capacity() >= init_size);
    }

    BOOST_FIXTURE_TEST_SUITE(resize_bigger, resize_vector_bigger)

        BOOST_AUTO_TEST_CASE(size_and_capacity_after_resize_bigger)
        {
            BOOST_TEST(v.size() == bigger_size);
            BOOST_TEST(v.capacity() >= bigger_size);
        }

    BOOST_AUTO_TEST_SUITE_END()
    
    BOOST_FIXTURE_TEST_SUITE(resize_smaller, resize_vector_smaller)

        BOOST_AUTO_TEST_CASE(size_and_capacity_after_resize_smaller)
        {
            BOOST_TEST(v.size() == smaller_size);
            BOOST_TEST(v.capacity() >= init_size);
        }

    BOOST_AUTO_TEST_SUITE_END()
    
BOOST_AUTO_TEST_SUITE_END()
