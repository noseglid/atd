#define _USE_MATH_DEFINES

#include "Math.h"

#include <cmath>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

float
deg2rad(const float& deg)
{
  return deg * (M_PI / 180.0f);
}

float
rad2deg(const float& rad)
{
  return rad * (180.0f / M_PI);
}

float
round(const float& number)
{
  return floorf(number * 100 + 0.5) / 100;
}
