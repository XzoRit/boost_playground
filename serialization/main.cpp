#include "GpsPosition.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cassert>

int main(int argc, char *argv[])
{
  using boost::archive::text_oarchive;
  using boost::archive::text_iarchive;
  
  std::cout << "\nboost_serialization\n\n";
    
  std::ofstream out("ar");
  const GpsPosition gps(1, 2, 3.0f);
  std::cout << "about to serialize gps = " << gps << '\n';
  {
    boost::archive::text_oarchive o(out);
    o << gps;
  }

  {
    std::ifstream in("ar");
    const std::string archiveData = std::string(std::istreambuf_iterator<char>(in),
						std::istreambuf_iterator<char>());
    std::cout << "data in archive\n";
    std::cout << archiveData << '\n';
  }
  std::ifstream in("ar");
  GpsPosition newGps;
  {
    text_iarchive i(in);
    i >> newGps;
    std::cout << "read back from file newGps = " << newGps << '\n';
  }

  assert(gps == newGps);
  
  return 0;
}
