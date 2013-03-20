#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Map.h"
#include "Tower.h"

#include <vector>

class TowerManager
{
	Map *map;
	std::map<Vector3, Tower*> towers;
	struct {
		int x, y;
	} click;

	void purchase_tower(Vector3 pos);

public:
	TowerManager(Map *map);
	void mouseup();
	void mousedown();
	void tick();
};

#endif
