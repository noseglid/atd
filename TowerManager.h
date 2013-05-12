#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"
#include "Map.h"
#include "Audio.h"
#include "Faction.h"

#include <vector>

struct GameEvent;

class TowerManager
{
	Vector3 dummy_tower_pos;
	std::string selected_tower;
	std::map<std::string, Tower*> dummy_towers;
	std::map<std::string, Json::Value> available_towers;

	std::map<Vector3, Tower*, Vector3cmp> towers;
	struct {
		int x, y;
	} click;

	MapEvent last_map_event;

	Mix_Chunk *audio_build;

	Tower *create_tower(std::string tower, Vector3 pos);
	void dummy_tower(int x, int y);
	bool purchase_tower(Vector3 pos);
	void deselect_tower();
	void select_tower(std::string name, int i);
	void map_hover(const MapEvent& me);

	void mouseup(const GameEvent& ev);
	void mousedown(const GameEvent& ev);
	void keydown(const GameEvent& ev);
	void tick(const GameEvent& ev);

public:
	TowerManager();
	void set_faction(Faction::Faction faction);

	static TowerManager& instance();
};

#endif
