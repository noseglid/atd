#ifndef __TARGETABLE_H__
#define __TARGETABLE_H__

#include "Math.h"
class Projectile;

class Targetable
{
public:
	virtual Vector3 get_position() const = 0;
	virtual void strike(Projectile *p) = 0;
};

#endif
