#ifndef __MATRIX4_H__
#define __MATRIX4_H__

#include "Vector3.h"

struct Matrix4
{
	float c1r1, c1r2, c1r3, c1r4;
	float c2r1, c2r2, c2r3, c2r4;
	float c3r1, c3r2, c3r3, c3r4;
	float c4r1, c4r2, c4r3, c4r4;

	Matrix4();
	Matrix4(
		float c1r1, float c1r2, float c1r3, float c1r4,
		float c2r1, float c2r2, float c2r3, float c2r4,
		float c3r1, float c3r2, float c3r3, float c3r4,
		float c4r1, float c4r2, float c4r3, float c4r4
	);

	Matrix4 operator*(const Matrix4& m);
	Vector3 operator*(const Vector3& v);
	Matrix4 operator*(const float& s);

	Vector3 translation() const;

	static Matrix4 rotatex(const float& rad);
	static Matrix4 rotatey(const float& rad);
	static Matrix4 rotatez(const float& rad);
};

std::ostream& operator<<(std::ostream& out, Matrix4 m);

#endif
