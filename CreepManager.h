#ifndef __CREEPMANAGER_H__
#define __CREEPMANAGER_H__

#include "Creep.h"
#include "Listener.h"

#include <vector>

class CreepManager : public Listener
{
	Map *map;
	float last_spawn;
	unsigned int spawned;
	std::vector<Creep*> creeps;

public:
	CreepManager(Map *map);

	void event(const float& elapsed);
};

#endif
