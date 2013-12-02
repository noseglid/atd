#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"
#include "engine/Event.h"
#include "Map.h"
#include "Audio.h"
#include "Faction.h"
#include "hud/InfoBox.h"
#include "hud/Button.h"
#include "gl/ShaderProgram.h"
#include "gl/glm.h"

#include <vector>

class TowerManager
{
  typedef std::map<glm::vec3, Tower*, gl::glmvec3cmp > tlist_t;

  std::vector<engine::Engine::id_t> events;

  glm::vec3 dummy_tower_pos;
  gl::ShaderProgram *dummy_tower_shader;
  std::string build_tower;
  std::map<std::string, Tower*> dummy_towers;
  std::map<std::string, Json::Value> available_towers;

  tlist_t::iterator selected_tower;
  tlist_t towers;
  glm::vec3 click;

  HUD::Button *btnupgr, *btnsell;

  std::map<HUD::Button*, HUD::InfoBox*> browseboxes;
  HUD::InfoBox *towerinfo, *upgradeinfo, *sellinfo;

  MapEvent last_map_event;

  Mix_Chunk *audio_build;

  TowerManager(const TowerManager& rhs);
  void operator=(const TowerManager& rhs);

  Tower *create_tower(std::string tower, glm::vec3 pos);
  void dummy_tower(int x, int y);
  bool purchase_tower(glm::vec3 pos);
  void upgrade_tower();
  void sell_tower();
  void build_tower_unset();

  void prepare_tower(std::string tower, HUD::Button *button);
  void button_mouse_event(bool on, Json::Value spec, HUD::Button *button);

  void map_hover(const MapEvent& me);

  void set_tower_infobox();
  void set_upgrade_infobox();
  void set_sell_infobox();
  void update_hud();

  int upgrades_left(tlist_t::const_iterator tower) const;
  bool tower_purchase_if();
  void tower_select_if(int clickx, int clicky);
  void select_tower(tlist_t::iterator it);
  void reselect_tower();

  void mouseup(const engine::Event& ev);
  void mousedown(const engine::Event& ev);
  void keydown(const engine::Event& ev);
  void tick(const engine::Event& ev);
  void tick_nodepth(const engine::Event& ev);

public:
  TowerManager();
  ~TowerManager();

  void set_faction(Faction::Faction faction);

  bool is_building() const;
  bool has_selected() const;

  bool escape();
};

#endif
