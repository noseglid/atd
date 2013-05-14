#include "Mobile.h"
#include "Debug.h"

Mobile::Mobile(float velocity) : last_elapsed(-1.0), velocity(velocity)
{
}

float
Mobile::get_speed_factor(const float& elapsed)
{
  float factor = (0.0f > last_elapsed) ? 0.0 : velocity * (elapsed - last_elapsed);
  last_elapsed = elapsed;
  return factor;
}
