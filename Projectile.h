#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "Creep.h"

#include <de.h>

class Projectile : public de::Emitter
{
	Creep *target;
	Vector3 pos;
	float damage;

	Emitter::id_t death_event;

public:
	Projectile(Creep *target, Vector3 pos, float damage);
	virtual ~Projectile();
	void set_target(Creep *target);

	void creep_death();
	void tick();
	void draw() const;

};

#endif
