#pragma once

#include <boost/serialization/access.hpp>
#include <ostream>

class GpsPosition
{
public:
  GpsPosition();
  GpsPosition(int d, int m, float s);

  int degrees() const;
  int minutes() const;
  float seconds() const;

private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & m_degrees;
    ar & m_minutes;
    ar & m_seconds;
  }
  
  int m_degrees;
  int m_minutes;
  float m_seconds;
};

bool operator==(const GpsPosition&, const GpsPosition&);
bool operator!=(const GpsPosition&, const GpsPosition&);
std::ostream& operator<<(std::ostream&, const GpsPosition&);
