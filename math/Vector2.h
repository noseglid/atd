#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include <iostream>

struct Vector2
{
  float x, y;

  Vector2();
  Vector2(float x, float y);

  void rotate(float rad);

  float length() const;
};

std::ostream& operator<<(std::ostream& out, Vector2 v);

#endif

