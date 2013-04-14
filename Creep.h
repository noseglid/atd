#ifndef __CREEP_H__
#define __CREEP_H__

#include "Map.h"
#include "Math.h"
#include "Targetable.h"
#include "Projectile.h"

#include <de.h>

class Creep : public de::Emitter<>, public Targetable
{
	friend class CreepManager;

	void travel_to(const PathCoord& target);

protected:
	Map *map;
	const Path *path;

	PathCoord target;
	Vector3 vtarget;

	float total_health, current_health;
	int reward;
	int life_cost;

	Vector3 pos, dir;

	Creep(Map *map, float health, int reward, int life_cost);

public:
	virtual ~Creep();

	virtual void struck(Projectile *p);
	Vector3 get_position() const;

	float get_health() const;

	virtual void draw(const float& elapsed) const;
	virtual void draw_model(const float& elapsed) const = 0;
	virtual void draw_health() const;
	void tick(const float& elapsed);
};

#endif
