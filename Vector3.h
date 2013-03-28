#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <iostream>

struct Vector3
{
	float x, y, z;

	Vector3();
	Vector3(float ix, float iy, float iz);
	Vector3(const Vector3& rhs);

	void normalize();
	float length() const;

	Vector3& operator*=(const float& rhs);
	Vector3& operator/=(const float& rhs);
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(const Vector3 &rhs);
	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator*(const Vector3& rhs) const;
	Vector3 operator*(const double& rhs) const;
	Vector3 operator/(const float& rhs) const;

	bool operator<(const Vector3& rhs) const;
};

std::ostream& operator<<(std::ostream& out, Vector3 v);

#endif
