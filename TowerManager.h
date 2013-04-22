#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"

#include <vector>

struct GameEvent;

class TowerManager
{
	enum TOWER_TYPE {
		TOWER_NONE,
		TOWER_ARCHERY,
		TOWER_CANNON
	} current_tower;

	std::map<Vector3, Tower*, Vector3cmp> towers;
	struct {
		int x, y;
	} click;

	void purchase_tower(Vector3 pos);
	void select_tower(TOWER_TYPE t, int i);

public:
	TowerManager();
	void mouseup(const GameEvent& ev);
	void mousedown(const GameEvent& ev);
	void keydown(const GameEvent& ev);
	void tick(const GameEvent& ev);

	static TowerManager& instance();
};

#endif
