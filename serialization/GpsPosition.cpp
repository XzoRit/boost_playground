#include "GpsPosition.hpp"

GpsPosition::GpsPosition()
  : m_degrees(0)
  , m_minutes(0)
  , m_seconds(0.0f)
{}

GpsPosition::GpsPosition(int d, int m, float s)
  : m_degrees(d)
  , m_minutes(m)
  , m_seconds(s)
{}

int GpsPosition::degrees() const
{
  return m_degrees;
}

int GpsPosition::minutes() const
{
  return m_minutes;
}

float GpsPosition::seconds() const
{
  return m_seconds;
}

std::ostream& operator<<(std::ostream& o, const GpsPosition& gps)
{
  o << "d:" << gps.degrees()
    << "m:" << gps.minutes()
    << "s:" << gps.seconds();

  return o;
}
