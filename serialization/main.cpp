#include "GpsPosition.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
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

namespace xzr
{
  class string_oarchive
  {
  public:
    typedef boost::mpl::bool_<true> is_saving; 
    typedef boost::mpl::bool_<false> is_loading;

    template<class T>
    string_oarchive& operator<<(const T& t)
    {
      m_stream << t;
      return *this;
    }
    
    template<class T>
    string_oarchive& operator&(const T& t)
    {
      return *this << t;
    }
    
    string_oarchive(std::ostream& str, unsigned int flags = 0)
      : m_stream(str)
    {}
  private:
    std::ostream& m_stream;
  };

  class string_iarchive
  {
  public:
    typedef boost::mpl::bool_<false> is_saving; 
    typedef boost::mpl::bool_<true> is_loading;

    template<class T>
    string_iarchive& operator>>(T& t)
    {
      return *this;
    }
    
    template<class T>
    string_iarchive& operator&(const T& t)
    {
      return *this >> t;
    }
    
    string_iarchive(std::istream& str)
      : m_stream(str)
    {}
  private:
    std::istream& m_stream;
  };
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
  std::cout << "\nxzr::string_archive\n";
  {
    const GpsPosition gps(1, 2, 3.0f);
    std::stringstream str;
    xzr::string_oarchive oar(str);
  }
  
  return 0;
}
