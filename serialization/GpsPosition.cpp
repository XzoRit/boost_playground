#include "GpsPosition.hpp"

GpsPosition::GpsPosition()
  : m_degrees(0)
  , m_minutes(0)
  , m_seconds(0.0f)
  , m_serialized(false)
{}

GpsPosition::GpsPosition(int d, int m, float s)
  : m_degrees(d)
  , m_minutes(m)
  , m_seconds(s)
  , m_serialized(false)
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

bool GpsPosition::serialized() const
{
  return m_serialized;
}

bool operator==(const GpsPosition& lhs, const GpsPosition& rhs)
{
  return ((lhs.degrees() == rhs.degrees())
    && (lhs.minutes() == rhs.minutes())
    && (lhs.seconds() == rhs.seconds()));
}

bool operator!=(const GpsPosition& lhs, const GpsPosition& rhs)
{
  return !(lhs == rhs);
}
