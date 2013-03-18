#include "Vector3.h"

#include <cmath>

Vector3::Vector3() : Vector3(0.0f, 0.0f, 0.0f)
{
}

Vector3::Vector3(float ix, float iy, float iz) : x(ix), y(iy), z(iz)
{
}

Vector3::Vector3(const Vector3& rhs) : Vector3(rhs.x, rhs.y, rhs.z)
{
}

float
Vector3::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

void
Vector3::normalize()
{
	float len = length();
	x = x / len;
	y = y / len;
	z = z / len;
}

Vector3&
Vector3::operator/=(const float& rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}

Vector3
Vector3::operator+(const Vector3& rhs) const
{
	return Vector3(
		x + rhs.x,
		y + rhs.y,
		z + rhs.z
	);
}

Vector3&
Vector3::operator+=(const Vector3& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Vector3
Vector3::operator-(const Vector3& rhs) const
{
	return Vector3(
		x - rhs.x,
		y - rhs.y,
		z - rhs.z
	);
}

Vector3&
Vector3::operator-=(const Vector3& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}

Vector3&
Vector3::operator*=(const Vector3 &rhs)
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}

Vector3
Vector3::operator*(const Vector3& rhs) const
{
	return Vector3(
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x
	);
}

Vector3
Vector3::operator*(const double& rhs) const
{
	return Vector3(
		x * rhs,
		y * rhs,
		z * rhs
	);
}

Vector3
Vector3::operator/(const float& rhs) const
{
	return Vector3(
		x/rhs,
		y/rhs,
		z/rhs
	);
}

bool
Vector3::operator<(const Vector3& rhs) const
{
	return this->length() < rhs.length();
}

std::ostream&
operator<<(std::ostream& out, Vector3 v)
{
	out.precision(2);
	return out << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}
