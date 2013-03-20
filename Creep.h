#ifndef __CREEP_H__
#define __CREEP_H__

#include "Map.h"
#include "Math.h"
#include "Targetable.h"
#include "Projectile.h"

#include <de.h>

class Creep : public de::Emitter, public Targetable
{
	void travel_to(const PathCoord& target);

protected:
	Map *map;
	const Path *path;
	PathCoord target;
	float total_health, current_health;
	int reward;

	Vector3 pos, dir;

	Creep(Map *map, float health, int reward);

public:
	virtual ~Creep();

	virtual void strike(Projectile *p);
	Vector3 get_position() const;

	float get_health() const;

	virtual void draw(const float& elapsed) const;
	virtual void draw_model(const float& elapsed) const = 0;
	virtual void draw_health() const;
	void tick(const float& elapsed);
};

#endif
