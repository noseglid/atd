#include "TowerManager.h"
#include "GLTransform.h"
#include "GLShapes.h"
#include "IO.h"
#include "ImageLoader.h"
#include "Text.h"
#include "Player.h"
#include "Creep.h"
#include "Game.h"
#include "HUD.h"
#include "Faction.h"

#define GOLD_COLOR 1.0f, 0.9f, 0.0f

TowerManager::TowerManager() : selected_tower(towers.end())
{
  Game& g = Game::instance();
  g.on("tick",      std::bind(&TowerManager::tick,      this, std::placeholders::_1));
  g.on("mousedown", std::bind(&TowerManager::mousedown, this, std::placeholders::_1));
  g.on("mouseup",   std::bind(&TowerManager::mouseup,   this, std::placeholders::_1));
  g.on("keydown",   std::bind(&TowerManager::keydown,   this, std::placeholders::_1));

  Map& m = Map::instance();
  m.on("hover", std::bind(&TowerManager::map_hover, this, std::placeholders::_1));

  audio_build = Audio::instance().load_sfx("sfx/build1.ogg");

}

void
TowerManager::set_faction(Faction::Faction faction)
{
  std::list<std::string> specs = Faction::Loader::tower_specs(faction);
  for (std::string file : specs) {
    Json::Value jspec = Json::deserialize(IO::file_get_contents(file));
    available_towers.insert(std::make_pair(jspec["name"].asString(), jspec));
    HUD::instance().add_button(
      IL::GL::texture(jspec["hudtex"].asString()),
      std::bind(
        &TowerManager::build_tower_set,
        this,
        jspec["name"].asString(),
        std::placeholders::_1
      )
    );
    Tower *t = create_tower(jspec["name"].asString(), Vector3());
    dummy_towers.insert(std::make_pair(jspec["name"].asString(), t));
  }
}

Tower *
TowerManager::create_tower(std::string tower, Vector3 pos)
{
  Json::Value spec = available_towers.at(tower);
  return new Tower(spec, pos);
}

void
TowerManager::dummy_tower(int x, int y)
{
  Map& map = Map::instance();
  Vector3 pos = map.get_center_of(x, y);
  pos.y = 0.0f;
  int hlx = x, hly = y;

  if (map.get_path()->has_coord(x, y) ||
      towers.end() != towers.find(pos) ||
      x == 0 || x == map.get_width() - 1 ||
      y == 0 || y == map.get_height() - 1) {
    /* This is not a valid coord for a dummy tower, reset all params */
    pos = Vector3(0.0f, 0.0f, 0.0f);
    hlx = -1;
    hly = -1;
  }

  dummy_tower_pos = pos;
  map.set_highlight(hlx, hly);
}

void
TowerManager::map_hover(const MapEvent& me)
{
  last_map_event = me;
  if (build_tower.empty())
    return;

  dummy_tower(me.hovered.x, me.hovered.y);
}

void
TowerManager::build_tower_unset()
{
  build_tower.erase();
  HUD::instance().mark_button(-1);
  Map::instance().set_highlight(-1, -1);
}

void
TowerManager::build_tower_set(std::string tower, int i)
{
  build_tower = tower;
  HUD::instance().mark_button(i);
  Map::instance().set_highlight(-1, -1);
  dummy_tower(last_map_event.hovered.x, last_map_event.hovered.y);
}

bool
TowerManager::purchase_tower(Vector3 pos)
{
  if (towers.end() != towers.find(pos)) {
    return false;
  }

  Tower *t = create_tower(build_tower, pos);
  if (!Player::instance().purchase(t)) {
    Text::set_color(1.0f, 1.0f, 1.0f);
    Text::scrolling("You're broke mate!", pos);
    delete t;
    return false;
  }

  towers.insert(std::make_pair(pos, t));
  dummy_tower_pos = Vector3();
  Map::instance().set_highlight(-1, -1);

  std::stringstream ss;
  ss << "-" << t->price << "g";
  Text::set_color(GOLD_COLOR);
  Text::scrolling(ss.str(), Vector3(pos.x, pos.y + 1.0f, pos.z));
  return true;
}

int
TowerManager::upgrades_left(tlist_t::const_iterator tower) const
{
  auto avail_it = available_towers.find(tower->second->get_name());
  if (available_towers.end() == avail_it) {
    DBGERR("No tower named: '" << selected_tower->second->get_name() << "'");
    return -1;
  }

  return (avail_it->second["upgrades"].asArray().size() - (tower->second->get_level() - 1));
}

void
TowerManager::upgrade_tower()
{
  if (0 == upgrades_left(selected_tower)) {
    return;
  }

  Tower *t = selected_tower->second;
  Json::Value upgrade = available_towers
    .find(selected_tower->second->get_name())
    ->second["upgrades"].asArray().at(t->get_level() - 1);

  Vector3 textpos = t->get_position();
  textpos.y += 1.0f;

  Purchasable dummy(upgrade["price"].asInt());
  if (!Player::instance().purchase(&dummy)) {
    Text::scrolling("No moneys for upgrade.", textpos);
    return;
  }

  t->upgrade(
    t->get_level() + 1,
    upgrade["reload"].asNumber(),
    upgrade["range"].asNumber(),
    upgrade["damage"].asNumber()
  );
  t->price += dummy.price;

  std::stringstream ss;
  ss << "-" << upgrade["price"].asInt() << "g";
  Text::set_color(GOLD_COLOR);
  Text::scrolling(ss.str(), textpos);

  /* Reset buttons so potentially disabled upgrade button is shown */
  tower_update_hud();
}

void
TowerManager::sell_tower()
{
  int return_value = Player::instance().sell(selected_tower->second);

  Vector3 textpos = selected_tower->second->get_position();
  textpos.y += 1.0f;

  std::stringstream ss;
  ss << "+" << return_value << "g";
  Text::set_color(GOLD_COLOR);
  Text::scrolling(ss.str(), textpos);

  towers.erase(selected_tower);
  selected_tower = towers.end();
  tower_update_hud();
}

void
TowerManager::keydown(const GameEvent& ge)
{
  switch (ge.ev.key.keysym.sym) {
  case SDLK_ESCAPE:
    selected_tower = towers.end();
    build_tower_unset();
    tower_update_hud();
    break;
  default:
    break;
  }
}

void
TowerManager::mousedown(const GameEvent& ev)
{
  SDL_MouseButtonEvent event = ev.ev.button;

  if (event.button != SDL_BUTTON_LEFT || HUD::instance().in_turf(event.x, event.y)) {
    return;
  }

  click.x = event.x;
  click.z = event.y;

}

bool
TowerManager::tower_purchase_if()
{
  Vector2 hl = Map::instance().get_highlight();
  if (0.0f >= hl.x || 0.0f >= hl.y) return false;

  Vector3 pos = Map::instance().get_center_of(hl.x, hl.y);
  pos.y = 0.0f;
  bool purchased = purchase_tower(pos);
  if (purchased) {
    Audio::instance().play(audio_build, 3);
  }

  SDLMod mod = SDL_GetModState();
  if (!(mod & KMOD_SHIFT) && purchased) {
    build_tower_unset();
  }

  return true;
}

void
TowerManager::tower_update_hud()
{
  static std::vector<int> added_buttons;
  static HUD& hud = HUD::instance();
  static HUD::BUTTON_LOCATION bloc = HUD::BOTTOM_RIGHT;

  HUD::instance().set_title("");

  added_buttons.erase(
    std::remove_if(
      added_buttons.begin(),
      added_buttons.end(),
        [](int buttonid) {
          HUD::instance().remove_button(buttonid);
          return true;
        }
      ),
    added_buttons.end()
  );

  if (towers.end() == selected_tower) {
    return;
  }

  std::string tex_upgr = (0 >= upgrades_left(selected_tower)) ?
    "upgrade_disabled.jpg" : "upgrade.jpg";

  added_buttons.push_back(hud.add_button(
    IL::GL::texture(tex_upgr),
    std::bind( &TowerManager::upgrade_tower, this),
    bloc
  ));

  added_buttons.push_back(hud.add_button(
    IL::GL::texture("sell.jpg"),
    std::bind( &TowerManager::sell_tower, this),
    bloc
  ));

  hud.set_title(selected_tower->second->get_name());
}

void
TowerManager::tower_select_if(int clickx, int clicky)
{
  try {
    Vector3 pos3d = GLTransform::unproject(clickx, clicky);
    Vector3 search = Map::instance().get_center_of(pos3d.x, pos3d.z);
    search.y = 0.0f;

    selected_tower = towers.find(search);

  } catch (const Exception& e) {
    /* clicked somewhere not part of map. just ignore */
    selected_tower = towers.end();
  }

  tower_update_hud();
}

void
TowerManager::mouseup(const GameEvent& ge)
{
  SDL_MouseButtonEvent event = ge.ev.button;

  if (HUD::instance().in_turf(event.x, event.y)) return;

  if (event.button == SDL_BUTTON_LEFT &&
      abs(event.x - click.x) < 3 &&
      abs(event.y - click.z) < 3) {

    /* Since coordinates didn't change (granted, some fuzz),
     * it was a `click`, not a `drag` */

    if (tower_purchase_if()) {
      /* If we managed to purchase a tower, do not immediately select it */
      return;
    } else {
      tower_select_if(event.x, event.y);
    }
  }
}

void
TowerManager::tick(const GameEvent& ev)
{
  float elapsed = ev.elapsed;

  if (selected_tower != towers.end()) {
    glPushMatrix();
    selected_tower->second->draw_range_circle();
    glPopMatrix();
  }

  for (std::pair<Vector3, Tower*> t : towers) {
    t.second->shoot_if(elapsed);
    t.second->update_projectiles(elapsed);

    glPushMatrix();
    t.second->draw(elapsed);
    glPopMatrix();
  }

  if (!build_tower.empty() && dummy_tower_pos.length() > 0.0f) {
    Tower *t = dummy_towers.at(build_tower);
    t->set_position(dummy_tower_pos);

    glPushMatrix();
      t->draw(elapsed, 0.5f);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
  }
}

TowerManager&
TowerManager::instance()
{
  static TowerManager instance;
  return instance;
}
