#ifndef __TOWER_H__
#define __TOWER_H__

#include "math/Math.h"
#include "Model.h"
#include "Creep.h"
#include "Projectile.h"
#include "Purchasable.h"

#include <de.h>
#include <list>

class Tower : public Purchasable
{
	Vector3 pos;
	std::list<Projectile *> projectiles;

	void draw_range_circle() const;
	Creep * get_target();

protected:
	float reload_time, range, damage, last_shot;
	Model *model;
	Tower(int price, Vector3 pos, float reload_time, float range, float damage);

public:
	void draw(const float& elapsed) const;

	void set_position(Vector3 pos);
	Vector3 get_position() const;
	void remove_projectile(Projectile *p);
	void projectile_hit(Projectile *projectile, Creep *creep,
		de::Emitter<>::id_t deathev, de::Emitter<>::id_t accev);
	void projectile_notarget(Projectile *p);
	void update_projectiles(const float& elapsed);
	void shoot_if(const float& elapsed);
};

#endif
