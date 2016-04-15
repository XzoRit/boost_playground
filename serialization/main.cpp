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

template<class OArchive, class IArchive>
GpsPosition saveLoad(const std::string& name, const GpsPosition& gps)
{
    std::cout << "about to save gps = " << gps << '\n';
    save<OArchive>(name, gps);
    printArchive(name);
    const GpsPosition newGps = load<IArchive>(name);
    std::cout << "loaded newGps = " << newGps << '\n';
    return newGps;
}

int main(int argc, char *argv[])
{
  using boost::archive::text_oarchive;
  using boost::archive::text_iarchive;
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;
  
  std::cout << "\nboost_serialization\n\n";
  std::cout << "text_archive\n";
  {
    const GpsPosition gps(1, 2, 3.0f);
    const GpsPosition newGps = saveLoad<text_oarchive, text_iarchive>("tar", gps);
    assert(gps == newGps);
  }
  std::cout << "\nbinary_archive\n";
  {
    const GpsPosition gps(1, 2, 3.0f);
    const GpsPosition newGps = saveLoad<binary_oarchive, binary_iarchive>("bar", gps);
    assert(gps == newGps);
  }
  
  return 0;
}
