#include "CreepManager.h"
#include "BoxCreep.h"
#include "Debug.h"
#include "Game.h"

#define SPAWN_INTERVAL 1.0f
#define COUNT 5

CreepManager::CreepManager(Map *map) : map(map), last_spawn(0), spawned(0)
{
	Game::instance().register_listener(this) << LE_TICK;
}

void
CreepManager::event(const float& elapsed)
{
	if ((elapsed > last_spawn + SPAWN_INTERVAL) && spawned < COUNT) {
		++spawned;
		creeps.push_back(new BoxCreep(map));
		last_spawn = elapsed;
	}

	for (Creep *c : creeps) {
		glPushMatrix();
		c->tick(elapsed);
		c->draw(elapsed);
		glPopMatrix();
	}
}
