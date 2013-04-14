#ifndef __TARGETABLE_H__
#define __TARGETABLE_H__

#include "math/Math.h"

class Projectile;

class Targetable
{
public:
	virtual Vector3 get_position() const = 0;
	virtual void struck(Projectile *p) = 0;
};

#endif
