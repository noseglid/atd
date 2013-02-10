#ifndef ___MATH__H__ // double underline so to not collide with <math.h> header.
#define ___MATH__H__

#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4.h"

float deg2rad(const float deg);
Vector3 unproject(const int& x, const int& y);

#endif
