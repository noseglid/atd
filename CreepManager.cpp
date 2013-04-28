#include "CreepManager.h"
#include "SphereCreep.h"
#include "Player.h"
#include "Text.h"
#include "Debug.h"
#include "Game.h"

#define SPAWN_INTERVAL 1.0f
#define COUNT 100

CreepManager&
CreepManager::instance()
{
	static CreepManager instance;
	return instance;
}

CreepManager::CreepManager() : last_spawn(-SPAWN_INTERVAL), spawned(0)
{
	Game::instance().on("tick", std::bind(&CreepManager::tick, this, std::placeholders::_1));
}

void
CreepManager::tick(const GameEvent& ev)
{
	float elapsed = ev.elapsed;
	if ((elapsed > last_spawn + SPAWN_INTERVAL) && spawned < COUNT) {
		++spawned;
		SphereCreep *creep = new SphereCreep();
		creep->on("death", std::bind(&CreepManager::creep_death, this, creep));
		creep->on("accomplished", std::bind(&CreepManager::creep_accomplished, this, creep));
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
CreepManager::creep_death(Creep *creep)
{
	Player::instance().alter_gold(creep->reward);

	std::stringstream ss;
	ss << creep->reward << "g";
	Text::scrolling(ss.str(), creep->get_position(), Vector3(1.0, 0.9, 0.0));

	remove_creep(creep);
}

void
CreepManager::creep_accomplished(Creep *creep)
{
	Player::instance().alter_lives(-(creep->life_cost));

	Text::scrolling("Haha!", creep->get_position(), Vector3(1.0, 1.0, 1.0));

	remove_creep(creep);
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
