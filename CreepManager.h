#ifndef __CREEPMANAGER_H__
#define __CREEPMANAGER_H__

#include "Creep.h"

#include <list>

class CreepManager
{
	Map *map;
	float last_spawn;
	unsigned int spawned;
	std::list<Creep*> creeps;

	CreepManager();
	void operator=(CreepManager const&);
	CreepManager(const CreepManager&);

	void remove_creep(Creep *creep);
	void tick(const GameEvent& ev);

public:
	void init(Map *map);

	void creep_death(Creep *creep);
	void creep_accomplished(Creep *creep);

	std::vector<Creep*> creeps_inside_circle(Vector3 center, float radius);

	static CreepManager& instance();
};

#endif
