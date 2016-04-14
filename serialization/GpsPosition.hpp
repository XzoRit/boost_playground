#pragma once

#include <boost/serialization/access.hpp>

class GpsPosition
{
public:
  GpsPosition();
  GpsPosition(int d, int m, float s);
  
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
