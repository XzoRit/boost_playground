cmake_minimum_required(VERSION 3.5)
project("boost_serialization")

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

find_package(Boost 1.59 REQUIRED COMPONENTS serialization)
include_directories(${Boost_INCLUDE_DIRS})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(boost_serialization main.cpp)
target_link_libraries(boost_serialization ${Boost_LIBRARIES})
