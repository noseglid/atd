#ifndef __MOBILE_H__
#define __MOBILE_H__

class Mobile
{
  float last_elapsed;
  float velocity;

protected:
  Mobile(float velocity);
  float get_speed_factor(const float& elapsed);
};

#endif
