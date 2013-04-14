#include "Vector2.h"

#include <cmath>

Vector2::Vector2() : x(0.0f), y(0.0f)
{
}

Vector2::Vector2(float x, float y) : x(x), y(y)
{
}

void
Vector2::rotate(float rad)
{
	float c = cos(rad);
	float s = sin(rad);

	float px = x * c - y * s;
	float py = x * s + y * c;

	x = px;
	y = py;
}

float
Vector2::length() const
{
	return sqrtf(x * x + y * y);
}

std::ostream& operator<<(std::ostream& out, Vector2 v)
{
	return out << '(' << v.x << ", " << v.y << ')';
}
