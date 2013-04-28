#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"
#include "Map.h"

#include <vector>

struct GameEvent;

class TowerManager
{
	enum TOWER_TYPE {
		/* Must be first */
		TOWER_FIRST,

		TOWER_ARCHERY,
		TOWER_CANNON,

		/* Must be last */
		TOWER_NONE
	} current_tower;

	Vector3 dummy_tower_pos;
	std::map<TOWER_TYPE, Tower*> dummy_towers;
	std::map<Vector3, Tower*, Vector3cmp> towers;
	struct {
		int x, y;
	} click;

	MapEvent last_map_event;

	Tower *create_tower(TOWER_TYPE t, Vector3 pos);
	void dummy_tower(int x, int y);
	bool purchase_tower(Vector3 pos);
	void select_tower(TOWER_TYPE t, int i = -1);
	void map_select(const MapEvent& me);

public:
	TowerManager();
	void mouseup(const GameEvent& ev);
	void mousedown(const GameEvent& ev);
	void keydown(const GameEvent& ev);
	void tick(const GameEvent& ev);

	static TowerManager& instance();
};

#endif
