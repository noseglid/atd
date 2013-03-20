#include "CreepManager.h"
#include "SphereCreep.h"
#include "Debug.h"
#include "Game.h"

#define SPAWN_INTERVAL 1.0f
#define COUNT 25

CreepManager&
CreepManager::instance()
{
	static CreepManager instance;
	return instance;
}

CreepManager::CreepManager() : last_spawn(-SPAWN_INTERVAL), spawned(0)
{
}

void
CreepManager::init(Map *map)
{
  this->map = map;

	Game::instance().register_listener(this) << LE_TICK;
}

void
CreepManager::event(const float& elapsed)
{
	if ((elapsed > (float)last_spawn + SPAWN_INTERVAL) && spawned < COUNT) {
		++spawned;
		SphereCreep *creep = new SphereCreep(map, 73);
		creep->on("death", std::bind(&CreepManager::remove_creep, this, creep));
		creeps.push_back(creep);
		last_spawn = elapsed;
	}

	std::list<Creep *> tmp = creeps;
	for (Creep *c : tmp) {
		glPushMatrix();
		c->draw(elapsed);
		glPopMatrix();

		c->tick(elapsed);
	}
}

void
CreepManager::remove_creep(Creep *creep)
{
	creeps.remove(creep);
	delete creep;
}

std::vector<Creep*>
CreepManager::creeps_inside_circle(Vector3 center, float radius)
{
	std::vector<Creep*> result;
	for (Creep *c : creeps) {
		Vector3 delta = c->get_position() - center;
		delta *= delta;
		if (delta.x + delta.z <= radius * radius && 0 < c->get_health())
			result.push_back(c);
	}

	return result;
}
