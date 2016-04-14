#include "GpsPosition.hpp"

GpsPosition::GpsPosition()
  : degrees(0)
  , minutes(0)
  , seconds(0.0f)
{}

GpsPosition::GpsPosition(int d, int m, float s)
  : degrees(d)
  , minutes(m)
  , seconds(s)
{}
