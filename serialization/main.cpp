#include "GpsPosition.hpp"
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
