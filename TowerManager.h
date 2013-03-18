#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Listener.h"
#include "Map.h"
#include "Tower.h"

#include <vector>

class TowerManager : public Listener
{
	Map *map;
	std::map<Vector3, Tower*> towers;

	void purchase_tower(Vector3 pos);

public:
	TowerManager(Map *map);
	void event(const SDL_MouseButtonEvent& event);
	void event(const float& elapsed);
};

#endif
