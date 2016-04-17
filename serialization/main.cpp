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
  return newGps;
  assert(gps == newGps);
}

namespace xzr
{
  template<class Archive>
  class string_oarchive_impl
    : public boost::archive::basic_text_oprimitive<std::ostream>
    , public boost::archive::basic_text_oarchive<Archive>
  {
  protected:
    friend class boost::archive::save_access;
  public:
    string_oarchive_impl(std::ostream& str, unsigned int flags)
      : boost::archive::basic_text_oprimitive<std::ostream>(str, flags)
      , boost::archive::basic_text_oarchive<Archive>(flags)
    {
      this->delimiter = boost::archive::basic_text_oarchive<Archive>::space;
    }

    template<class T>
    void save(const T& t){
        this->newtoken();
        basic_text_oprimitive<std::ostream>::save(t);
    }
  };
  
  class string_oarchive
    : public string_oarchive_impl<string_oarchive>
  {
  public:
    string_oarchive(std::ostream& str, unsigned int flags = 0)
      : string_oarchive_impl<string_oarchive>(str, flags)
      , m_stream(str)
    {}
  private:
    std::ostream& m_stream;
  };

  template<class Archive>
  class string_iarchive_impl
    : public boost::archive::basic_text_iprimitive<std::istream>
    , public boost::archive::basic_text_iarchive<Archive>
  {
  protected:
    friend class boost::archive::load_access;
  public:
    string_iarchive_impl(std::istream& str, unsigned int flags = 0)
      : boost::archive::basic_text_iprimitive<std::istream>(str, true)
      , boost::archive::basic_text_iarchive<Archive>(flags)
    {
    }
  };
  
  class string_iarchive
    : public string_iarchive_impl<string_iarchive>
  {
  public:
    string_iarchive(std::istream& str)
      : string_iarchive_impl<string_iarchive>(str)
      , m_stream(str)
    {}
  private:
    std::istream& m_stream;
  };
}

#include <boost/archive/impl/basic_text_oarchive.ipp>
#include <boost/archive/impl/basic_text_iarchive.ipp>

// BOOST_CLASS_TRACKING(GpsPosition, boost::serialization::track_never)

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/range/iterator_range.hpp>

int main(int argc, char *argv[])
{
  using boost::archive::text_oarchive;
  using boost::archive::text_iarchive;
  using boost::archive::binary_oarchive;
  using boost::archive::binary_iarchive;

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
  std::cout << "\nxzr::string_archive\n";
  {
    {
      const GpsPosition gps(1, 2, 3.0f);
      const GpsPosition newGps = saveLoad<xzr::string_oarchive, xzr::string_iarchive>("sar", gps);
    }
    std::cout << "serialize/deserialize into string\n";
    {
      namespace io = boost::iostreams;
      const GpsPosition gps(1, 2, 3.0f);
      std::string str;
      {
	io::filtering_ostream out(io::back_inserter(str));
	xzr::string_oarchive ar(out);
	ar & gps;
	assert(gps.serialized());
	out.flush();
	std::cout << "data in archive\n" << str << '\n';
      }
      {
	io::filtering_istream in(boost::make_iterator_range(str));
	xzr::string_iarchive ar(in);
	GpsPosition newGps;
	ar & newGps;
	assert(newGps.serialized());
	assert(gps == newGps);
      }
    }
  }
  
  return 0;
}
