#include "Matrix4.h"

#include <iomanip>
#include <cmath>

Matrix4::Matrix4()
{
	c1r1 = c1r2 = c1r3 = c1r4 = 0.0f;
	c2r1 = c2r2 = c2r3 = c2r4 = 0.0f;
	c3r1 = c3r2 = c3r3 = c3r4 = 0.0f;
	c4r1 = c4r2 = c4r3 = c4r4 = 0.0f;
}

Matrix4::Matrix4(
	float c1r1, float c1r2, float c1r3, float c1r4,
	float c2r1, float c2r2, float c2r3, float c2r4,
	float c3r1, float c3r2, float c3r3, float c3r4,
	float c4r1, float c4r2, float c4r3, float c4r4
) :
	c1r1(c1r1), c1r2(c1r2), c1r3(c1r3), c1r4(c1r4),
	c2r1(c2r1), c2r2(c2r2), c2r3(c2r3), c2r4(c2r4),
	c3r1(c3r1), c3r2(c3r2), c3r3(c3r3), c3r4(c3r4),
	c4r1(c4r1), c4r2(c4r2), c4r3(c4r3), c4r4(c4r4)
{
}

Matrix4
Matrix4::operator*(const Matrix4& m)
{
	Matrix4 res;

	res.c1r1 = c1r1 * m.c1r1 + c2r1 * m.c1r2 + c3r1 * m.c1r3 + c4r1 * m.c1r4;
	res.c1r2 = c1r2 * m.c1r1 + c2r2 * m.c1r2 + c3r2 * m.c1r3 + c4r2 * m.c1r4;
	res.c1r3 = c1r3 * m.c1r1 + c2r3 * m.c1r2 + c3r3 * m.c1r3 + c4r3 * m.c1r4;
	res.c1r4 = c1r4 * m.c1r1 + c2r4 * m.c1r2 + c3r4 * m.c1r3 + c4r4 * m.c1r4;

	res.c2r1 = c1r1 * m.c2r1 + c2r1 * m.c2r2 + c3r1 * m.c2r3 + c4r1 * m.c2r4;
	res.c2r2 = c1r2 * m.c2r1 + c2r2 * m.c2r2 + c3r2 * m.c2r3 + c4r2 * m.c2r4;
	res.c2r3 = c1r3 * m.c2r1 + c2r3 * m.c2r2 + c3r3 * m.c2r3 + c4r3 * m.c2r4;
	res.c2r4 = c1r4 * m.c2r1 + c2r4 * m.c2r2 + c3r4 * m.c2r3 + c4r4 * m.c2r4;

	res.c3r1 = c1r1 * m.c3r1 + c2r1 * m.c3r2 + c3r1 * m.c3r3 + c4r1 * m.c3r4;
	res.c3r2 = c1r2 * m.c3r1 + c2r2 * m.c3r2 + c3r2 * m.c3r3 + c4r2 * m.c3r4;
	res.c3r3 = c1r3 * m.c3r1 + c2r3 * m.c3r2 + c3r3 * m.c3r3 + c4r3 * m.c3r4;
	res.c3r4 = c1r4 * m.c3r1 + c2r4 * m.c3r2 + c3r4 * m.c3r3 + c4r4 * m.c3r4;

	res.c4r1 = c1r1 * m.c4r1 + c2r1 * m.c4r2 + c3r1 * m.c4r3 + c4r1 * m.c4r4;
	res.c4r2 = c1r2 * m.c4r1 + c2r2 * m.c4r2 + c3r2 * m.c4r3 + c4r2 * m.c4r4;
	res.c4r3 = c1r3 * m.c4r1 + c2r3 * m.c4r2 + c3r3 * m.c4r3 + c4r3 * m.c4r4;
	res.c4r4 = c1r4 * m.c4r1 + c2r4 * m.c4r2 + c3r4 * m.c4r3 + c4r4 * m.c4r4;

	return res;
}

Vector3
Matrix4::operator*(const Vector3& v)
{
	Vector3 res;
	res.x = c1r1 * v.x + c2r1 * v.y + c3r1 * v.z;
	res.y = c1r2 * v.x + c2r2 * v.y + c3r2 * v.z;
	res.z = c1r3 * v.x + c2r3 * v.y + c3r3 * v.z;

	return res;
}

Matrix4
Matrix4::operator*(const float& s)
{
	return Matrix4(
		c1r1 * s, c1r2 * s, c1r3 * s, c1r4 * s,
		c2r1 * s, c2r2 * s, c2r3 * s, c2r4 * s,
		c3r1 * s, c3r2 * s, c3r3 * s, c3r4 * s,
		c4r1 * s, c4r2 * s, c4r3 * s, c4r4 * s
	);
}

Vector3
Matrix4::translation() const
{
	return Vector3(c4r1, c4r2, c4r3);
}

Matrix4
Matrix4::rotatex(const float& rad)
{
	return Matrix4(
		1.0f, 0.0f,      0.0f,     0.0f,
		0.0f, cos(rad),  sin(rad), 0.0f,
		0.0f, -sin(rad), cos(rad), 0.0f,
		0.0f, 0.0f,      0.0f,     1.0f
	);
}

Matrix4
Matrix4::rotatey(const float& rad)
{
	return Matrix4(
		cos(rad),  0.0f, sin(rad), 0.0f,
		0.0f,      1.0f, 0.0f,     0.0f,
		-sin(rad), 0.0f, cos(rad), 0.0f,
		0.0f,      0.0f, 0.0f,     1.0f
	);
}

Matrix4
Matrix4::rotatez(const float& rad)
{
	return Matrix4(
		cos(rad),  sin(rad), 0.0f, 0.0f,
		-sin(rad), cos(rad), 0.0f, 0.0f,
		0.0f,      0.0f,     1.0f, 0.0f,
		0.0f,      0.0f,     0.0f, 1.0f
	);
}

std::ostream&
operator<<(std::ostream& out, Matrix4 m)
{
	using std::setw;
	using std::endl;
	int width = 8;
	out.precision(4);

	return out << std::setfill(' ') << std::setiosflags(std::ios::right) << std::fixed << std::endl <<
		setw(width) << m.c1r1 << " " << setw(width) << m.c2r1 << " " << setw(width) << m.c3r1 << " " << setw(width) << m.c4r1 << endl <<
		setw(width) << m.c1r2 << " " << setw(width) << m.c2r2 << " " << setw(width) << m.c3r2 << " " << setw(width) << m.c4r2 << endl <<
		setw(width) << m.c1r3 << " " << setw(width) << m.c2r3 << " " << setw(width) << m.c3r3 << " " << setw(width) << m.c4r3 << endl <<
		setw(width) << m.c1r4 << " " << setw(width) << m.c2r4 << " " << setw(width) << m.c3r4 << " " << setw(width) << m.c4r4;
}
