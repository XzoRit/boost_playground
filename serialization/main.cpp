#include <boost/serialization/access.hpp>

class GpsPosition
{
public:
  GpsPosition()
    : degrees(0)
    , minutes(0)
    , seconds(0.0f)
  {}

  GpsPosition(int d, int m, float s)
    : degrees(d)
    , minutes(m)
    , seconds(s)
  {}
  
private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & degrees;
    ar & minutes;
    ar & seconds;
  }
  
  int degrees;
  int minutes;
  float seconds;
};

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>

int main(int argc, char *argv[])
{
  std::ofstream out("ar");
  const GpsPosition gps(1, 2, 3.0f);

  {
    boost::archive::text_oarchive o(out);
    o << gps;
  }

  std::ifstream in("ar");
  GpsPosition newGps;
  {
    boost::archive::text_iarchive i(in);
    i >> newGps;
  }

  return 0;
}
