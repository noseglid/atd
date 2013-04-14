#ifndef __TOWER_H__
#define __TOWER_H__

#include "Math.h"
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
	float last_shot, reload_time, range, damage;

public:
	Tower(Vector3 pos);
	void draw(const float& elapsed) const;
	void remove_projectile(Projectile *p);
	void projectile_hit(Projectile *projectile, Creep *creep,
		de::Emitter<>::id_t deathev, de::Emitter<>::id_t accev);
	void projectile_notarget(Projectile *p);
	void update_projectiles(const float& elapsed);
	void shoot_if(const float& elapsed);

	static void init();
};

#endif
