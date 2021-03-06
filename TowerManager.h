#ifndef __TOWERMANAGER_H__
#define __TOWERMANAGER_H__

#include "Tower.h"
#include "engine/Event.h"
#include "map/Map.h"
#include "Audio.h"
#include "Faction.h"
#include "hud/InfoBox.h"
#include "hud/Button.h"
#include "gl/ShaderProgram.h"
#include "gl/glm.h"

#include <vector>

class towerposcmp
{
public:
  bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
  {
    if (lhs.z < rhs.z) return true;
    if (lhs.z == rhs.z && lhs.x < rhs.x) return true;
    return false;
  }
};

class TowerManager
{
  typedef std::map<glm::vec3, Tower*, towerposcmp > tlist_t;

  std::vector<engine::Engine::id_t> events;

  glm::vec3 dummy_tower_pos;
  gl::ShaderProgram *dummy_tower_shader;
  std::string build_tower;
  std::map<std::string, Tower*> dummy_towers;
  std::map<std::string, Json::Value> available_towers;

  tlist_t::iterator selected_tower;
  tlist_t towers;
  glm::vec3 click;

  hud::Button *btnupgr, *btnsell;

  std::map<hud::Button*, hud::InfoBox*> browseboxes;
  hud::InfoBox *towerinfo, *upgradeinfo, *sellinfo;

  map::MapEvent last_map_event;

  Mix_Chunk *audio_build;

  TowerManager(const TowerManager& rhs);
  void operator=(const TowerManager& rhs);

  Tower *create_tower(std::string tower, glm::vec3 pos);
  void dummy_tower(int x, int y);
  bool purchase_tower(glm::vec3 pos);
  void upgrade_tower();
  void sell_tower();
  void build_tower_unset();

  void prepare_tower(std::string tower, hud::Button *button);
  void button_mouse_event(bool on, Json::Value spec, hud::Button *button);

  void map_hover(const map::MapEvent& me);

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
