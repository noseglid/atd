#ifndef __GLTRANSFORM_H__
#define __GLTRANSFORM_H__

#include "math/Math.h"

class GLTransform
{
public:
	static void billboard();
	static void enable2D();
	static void disable2D();
	static Vector3 unproject(const int& x, const int& y);
};

#endif
