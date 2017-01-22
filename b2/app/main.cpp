#include "calculator.hpp"
#include <boost/program_options.hpp>
#include <cassert>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    std::string numbers_as_string;
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("numbers", po::value<std::string>(&numbers_as_string), "comma separated list of numbers");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }
    if (vm.count("numbers"))
    {
        std::cout << "sum of " << numbers_as_string
                  << " = " << Calculator::add(numbers_as_string)
                  << "\n";
    }

    return 0;
}
