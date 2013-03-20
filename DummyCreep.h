#ifndef __DUMMYCREEP_H__
#define __DUMMYCREEP_H__

#include "Targetable.h"

class DummyCreep : public Targetable
{
	Vector3 pos;

public:
	DummyCreep(Vector3 pos);
	virtual Vector3 get_position() const;
	virtual void strike(Projectile *p);
};

#endif

