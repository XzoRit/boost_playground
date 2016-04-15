#include "GpsPosition.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cassert>

template<class Archive>
void save(const std::string& name, const GpsPosition& gps)
{
  std::ofstream oFile(name);
  Archive ar(oFile);
  ar << gps;
}

template<class Archive>
GpsPosition load(const std::string& name)
{
  std::ifstream in(name);
  GpsPosition newGps;
  {
    Archive i(in);
    i >> newGps;
  }
  return newGps;
}

void printArchive(const std::string& name)
{
  std::ifstream in(name);
  const std::string archiveData = std::string(std::istreambuf_iterator<char>(in),
					      std::istreambuf_iterator<char>());
  std::cout << "data in archive " << name << "\n";
  std::cout << archiveData << '\n';
}

int main(int argc, char *argv[])
{
  using boost::archive::text_oarchive;
  using boost::archive::text_iarchive;
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;
  
  std::cout << "\nboost_serialization\n\n";

  {
  const GpsPosition gps(1, 2, 3.0f);
  std::cout << "about to save gps = " << gps << '\n';
  save<text_oarchive>("ar", gps);
  printArchive("ar");
  const GpsPosition newGps = load<text_iarchive>("ar");
  std::cout << "loaded newGps = " << newGps << '\n';
  assert(gps == newGps);
  }
  
  {
  const GpsPosition gps(1, 2, 3.0f);
  std::cout << "about to save gps = " << gps << '\n';
  save<binary_oarchive>("ar", gps);
  printArchive("ar");
  const GpsPosition newGps = load<binary_iarchive>("ar");
  std::cout << "loaded newGps = " << newGps << '\n';
  assert(gps == newGps);
  }
  
  return 0;
}
