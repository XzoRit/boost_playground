#pragma once

#include <boost/serialization/access.hpp>

class GpsPosition
{
public:
  GpsPosition();
  GpsPosition(int d, int m, float s);

  int degrees() const;
  int minutes() const;
  float seconds() const;
  bool serialized() const;

private:
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & m_degrees;
    ar & m_minutes;
    ar & m_seconds;
    m_serialized = true;
  }
  
  int m_degrees;
  int m_minutes;
  float m_seconds;
  bool m_serialized;
};

bool operator==(const GpsPosition&, const GpsPosition&);
bool operator!=(const GpsPosition&, const GpsPosition&);
