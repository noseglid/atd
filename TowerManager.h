#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"
#include "Map.h"
#include "Audio.h"
#include "Faction.h"
#include "hud/InfoBox.h"

#include <vector>

struct GameEvent;

class TowerManager
{
  typedef std::map<Vector3, Tower*, Vector3cmp> tlist_t;

  Vector3 dummy_tower_pos;
  std::string build_tower;
  std::map<std::string, Tower*> dummy_towers;
  std::map<std::string, Json::Value> available_towers;

  HUD::InfoBox::boxid_t boxid;
  tlist_t::iterator selected_tower;
  tlist_t towers;
  Vector3 click;

  MapEvent last_map_event;

  Mix_Chunk *audio_build;

  Tower *create_tower(std::string tower, Vector3 pos);
  void dummy_tower(int x, int y);
  bool purchase_tower(Vector3 pos);
  void upgrade_tower();
  void sell_tower();
  void build_tower_unset();
  void build_tower_set(std::string name, int i);
  void map_hover(const MapEvent& me);

  int upgrades_left(tlist_t::const_iterator tower) const;
  bool tower_purchase_if();
  void update_hud();
  void tower_select_if(int clickx, int clicky);
  void select_tower(tlist_t::iterator it);
  void reselect_tower();

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
