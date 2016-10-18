#include "calculator.hpp"
#include <boost/program_options.hpp>
#include <cassert>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("compression", po::value<int>(), "set compression level");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 0;
    }

    assert(Calculator::add("1,22,333") == 356);

    return 0;
}
