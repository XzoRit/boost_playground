#include "GpsPosition.hpp"
#include "string_iarchive.hpp"
#include "string_oarchive.hpp"
#include "bin_iarchive.hpp"
#include "bin_oarchive.hpp"
#include "binary_data_archive.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <../share/doc/boost/libs/serialization/example/simple_log_archive.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cassert>
#include <sstream>

template<class Archive>
void save(const std::string& name, const GpsPosition& gps)
{
  std::ofstream oFile(name);
  Archive ar(oFile);
  ar << gps;
  assert(gps.serialized());
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
  assert(newGps.serialized());
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
  save<OArchive>(name, gps);
  printArchive(name);
  const GpsPosition newGps = load<IArchive>(name);
  assert(gps == newGps);
  return newGps;
}

int main(int argc, char *argv[])
{
  using boost::archive::text_oarchive;
  using boost::archive::text_iarchive;
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;

  using xzr::archive::string_oarchive;
  using xzr::archive::string_iarchive;
  using xzr::archive::bin_oarchive;
  using xzr::archive::bin_iarchive;

  using RTT::marsh::binary_data_oarchive;
  using RTT::marsh::binary_data_iarchive;


  using boost::serialization::implementation_level;
  using boost::serialization::tracking_level;
  
  std::cout << "\nboost_serialization\n\n";
  
  std::cout << "GpsPosition implementation_level = "
	    << implementation_level<GpsPosition>::value << '\n'
	    << "GpsPosition tracking_level = "
	    << tracking_level<GpsPosition>::value << '\n';

  std::cout << "\ntext_archive\n";
  {
    const GpsPosition gps(1, 2, 3.0f);
    const GpsPosition newGps = saveLoad<text_oarchive, text_iarchive>("tar", gps);
  }
  std::cout << "\nbinary_archive\n";
  {
    const GpsPosition gps(1, 2, 3.0f);
    const GpsPosition newGps = saveLoad<binary_oarchive, binary_iarchive>("bar", gps);
  }
  std::cout << "\nsimple_log_archive\n";
  {
      const GpsPosition gps(1, 2, 3.0f);
      save<simple_log_archive>("lar", gps);
      printArchive("lar");
  }
  std::cout << "\nxzr::archive::string_archive\n";
  {
      const GpsPosition gps(1, 2, 3.0f);
      const GpsPosition newGps = saveLoad<string_oarchive, string_iarchive>("sar", gps);
  }
  std::cout << "\nxzr::archive::bin_archive\n";
  {
      const GpsPosition gps(1, 2, 3.0f);
      const GpsPosition newGps = saveLoad<bin_oarchive, bin_iarchive>("binar", gps);
  }
  std::cout << "\nRTT::marsh::binary_data_archive\n";
  {
      const GpsPosition gps(1, 2, 3.0f);
      const GpsPosition newGps = saveLoad<binary_data_oarchive, binary_data_iarchive>("bindatar", gps);
  }

  std::cout << "serialize/deserialize into string\n";
  {
    namespace io = boost::iostreams;
    const GpsPosition gps(1, 2, 3.0f);
    std::string str;
    {
      io::filtering_ostream out(io::back_inserter(str));
      string_oarchive ar(out);
      ar & gps;
      assert(gps.serialized());
      out.flush();
      std::cout << "data in archive\n" << str << '\n';
    }
    {
      io::filtering_istream in(boost::make_iterator_range(str));
      string_iarchive ar(in);
      GpsPosition newGps;
      ar & newGps;
      assert(newGps.serialized());
      assert(gps == newGps);
    }
  }
  
  return 0;
}
