#include "TowerManager.h"
#include "Debug.h"
#include "gl/Transform.h"
#include "IO.h"
#include "Game.h"
#include "ImageLoader.h"
#include "text/Text.h"
#include "text/Dispatcher.h"
#include "Player.h"
#include "Creep.h"
#include "engine/Engine.h"
#include "HUD/InfoBox.h"
#include "HUD/InfoBar.h"
#include "HUD/ButtonBar.h"
#include "HUD/Button.h"
#include "Faction.h"

#define TEXTURE_UPGRADE          "thrust.png"
#define TEXTURE_UPGRADE_DISABLED "thrust_gray.png"
#define TEXTURE_SELL             "cash.png"

#define CLICK_FUZZ 3

TowerManager::TowerManager() : selected_tower(towers.end()), btnupgr(NULL), btnsell(NULL)
{
  DBG("Registering events for TowerManager");
  engine::Engine& e = engine::Engine::instance();
  events.push_back(
    e.on("tick",         std::bind(&TowerManager::tick,         this, std::placeholders::_1))
  );
  events.push_back(
    e.on("tick_nodepth", std::bind(&TowerManager::tick_nodepth, this, std::placeholders::_1))
  );
  events.push_back(
    e.on("mousedown",    std::bind(&TowerManager::mousedown,    this, std::placeholders::_1))
  );
  events.push_back(
    e.on("mouseup",      std::bind(&TowerManager::mouseup,      this, std::placeholders::_1))
  );
  events.push_back(
    e.on("keydown",      std::bind(&TowerManager::keydown,      this, std::placeholders::_1))
  );

  Map *m = Game::instance().map;
  m->on("hover", std::bind(&TowerManager::map_hover, this, std::placeholders::_1));

  audio_build = Audio::instance().load_sfx("build1.ogg");

  towerinfo   = new hud::InfoBox(text::Stream(), hud::InfoBox::SNAP_TOPRIGHT);
  upgradeinfo = new hud::InfoBox(text::Stream(), hud::InfoBox::SNAP_BOTRIGHT, true);
  sellinfo    = new hud::InfoBox(text::Stream(), hud::InfoBox::SNAP_BOTRIGHT, true);

  dummy_tower_shader = gl::ShaderProgram::create({ "model.v.glsl" }, { "dummy_tower.f.glsl" });
}

TowerManager::~TowerManager()
{
  DBG("Deregistering TowerManager from events");
  for (engine::Engine::id_t ev : events) {
    engine::Engine::instance().off(ev);
  }

  delete towerinfo;
  delete upgradeinfo;
  delete sellinfo;
  delete dummy_tower_shader;

  hud::ButtonBar::instance().clear_buttons();
  btnupgr = NULL;
  btnsell = NULL;
}

void
TowerManager::set_faction(Faction::Faction faction)
{
  std::list<std::string> specs = Faction::Loader::tower_specs(faction);
  for (std::string file : specs) {
    Json::Value jspec = Json::deserialize(IO::file_get_contents(file));
    std::string name = jspec["desc"]["name"].asString();

    hud::Button *button = new hud::Button(IL::GL::texture(jspec["hudtex"].asString()));
    hud::ButtonBar::instance().add_button(button);
    button->on("leftclick",
               std::bind(&TowerManager::prepare_tower, this, name, std::placeholders::_1));
    button->on("mouseentry",
               std::bind(&TowerManager::button_mouse_event, this,
                         true, jspec, std::placeholders::_1));
    button->on("mouseexit",
               std::bind(&TowerManager::button_mouse_event, this,
                          false, jspec, std::placeholders::_1));

    available_towers.insert(std::make_pair(name, jspec));

    dummy_towers.insert(std::make_pair(name, create_tower(name, glm::vec3())));
  }
}

Tower *
TowerManager::create_tower(std::string tower, glm::vec3 pos)
{
  Json::Value spec = available_towers.at(tower);
  return new Tower(spec, pos);
}

void
TowerManager::dummy_tower(int x, int y)
{
  Map *map = Game::instance().map;
  glm::vec3 pos = map->get_center_of(x, y);
  pos.y = 0.0f;
  int hlx = x, hly = y;

  if (map->get_path()->has_coord(x, y) ||
      towers.end() != towers.find(pos) ||
      x == 0 || x == map->get_width() - 1 ||
      y == 0 || y == map->get_height() - 1) {
    /* This is not a valid coord for a dummy tower, reset all params */
    pos = glm::vec3(-200.0f, 0.0f, -200.0f);
    hlx = -1;
    hly = -1;
  }

  dummy_tower_pos = pos;
  map->set_highlight(hlx, hly);
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
  hud::ButtonBar::instance().unmark_all();
  Game::instance().map->set_highlight(-1, -1);
}

/**
 * Callback from clicking HUD button to build tower
 */
void
TowerManager::prepare_tower(std::string tower, hud::Button *button)
{
  /* Deselect any selected tower */
  build_tower_unset();
  select_tower(towers.end());

  build_tower = tower;
  button->mark();
  Game::instance().map->set_highlight(-1, -1);
  dummy_tower(last_map_event.hovered.x, last_map_event.hovered.y);
}

void
TowerManager::button_mouse_event(bool on, Json::Value spec, hud::Button *button)
{
  auto it = browseboxes.find(button);
  if (browseboxes.end() != it) {
    delete it->second;
    browseboxes.erase(it);
  }

  if (on) {
    using hud::InfoBox;
    std::string limit = spec["desc"].objectHasKey("limit") ?
      std::string("Limit: ") + spec["desc"]["limit"].asString() + "\n" : "";

    text::Stream ts;
    ts
      << text::Stream::size(32.0f) << utils::colors::orange
      << spec["desc"]["name"].asString() << "\n"
      << text::Stream::size(18.0f) << text::Stream::indent(8.0f) << utils::colors::lightgray
      << text::Text::linebreak(spec["desc"]["info"].asString()) << "\n"
      << utils::colors::red << limit << "\n"
      << text::Stream::size(16.0f) << text::Stream::indent(18.0f)
      << utils::colors::white << "Type: "        << utils::colors::green << spec["type"].asString()   << "\n"
      << utils::colors::white << "Price: "       << utils::colors::gold  << spec["price"].asNumber()
      << utils::colors::white << " g" << "\n"
      << utils::colors::white << "Base damage: " << utils::colors::green << spec["damage"].asNumber() << "\n"
      << utils::colors::white << "Base range: "  << utils::colors::green << spec["range"].asNumber()  << "\n"
      << utils::colors::white << "Base reload: " << utils::colors::green << spec["reload"].asNumber();

    browseboxes.insert(std::make_pair(button, new InfoBox(ts, InfoBox::SNAP_BOTLEFT, true)));
  }
}

bool
TowerManager::purchase_tower(glm::vec3 pos)
{
  if (towers.end() != towers.find(pos)) {
    return false;
  }

  Tower *t = create_tower(build_tower, pos);

  auto purchase_fail = [pos, t](std::string msg) {
    text::Dispatcher::instance().scrolling(msg, pos);
    delete t;
    return false;
  };

  if (!Game::instance().player->purchase(t)) {
    return purchase_fail("Not enough gold");
  }

  if (t->is_hero()) {
    /* Trying to purchase a hero tower. If we already have one, fail - only one is allowed. */
    tlist_t::iterator it = std::find_if(towers.begin(), towers.end(),
      [](std::pair<glm::vec3, Tower *> entry) {
        return entry.second->is_hero();
      }
    );

    if (it != towers.end()) return purchase_fail("Only one hero tower allowed");
  }

  towers.insert(std::make_pair(pos, t));
  dummy_tower_pos = glm::vec3(-200.0, 0.0, -200.0);
  Game::instance().map->set_highlight(-1, -1);

  std::stringstream ss;
  ss << "-" << t->price << "g";
  text::Dispatcher::instance().scrolling(ss.str(), glm::vec3(pos.x, pos.y + 1.0f, pos.z), utils::colors::gold);
  return true;
}

int
TowerManager::upgrades_left(tlist_t::const_iterator tower) const
{
  auto avail_it = available_towers.find(tower->second->get_name());
  if (available_towers.end() == avail_it) {
    /* this is weird, selected a tower which doesn't exist */
    DBGERR("No tower named: '" << selected_tower->second->get_name() << "'");
    return -1;
  }

  int total = avail_it->second["upgrades"].asArray().size();
  int used  = (tower->second->get_level() - 1);
  return (total - used);
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

  glm::vec3 textpos = t->get_position();
  textpos.y += 1.0f;

  Purchasable dummy(upgrade["price"].asInt());
  if (!Game::instance().player->purchase(&dummy)) {
    text::Dispatcher::instance().scrolling("No moneys for upgrade.", textpos);
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
  text::Dispatcher::instance().scrolling(ss.str(), textpos, utils::colors::gold);

  set_tower_infobox();
  set_upgrade_infobox();

  if (0 >= upgrades_left(selected_tower)) {
    btnupgr->set_texture(IL::GL::texture(TEXTURE_UPGRADE_DISABLED));
  }
}

void
TowerManager::sell_tower()
{
  int return_value = Game::instance().player->sell(selected_tower->second);
  sellinfo->clear();

  glm::vec3 textpos = selected_tower->second->get_position();
  textpos.y += 1.0f;

  std::stringstream ss;
  ss << "+" << return_value << "g";
  text::Dispatcher::instance().scrolling(ss.str(), textpos, utils::colors::gold);

  towers.erase(selected_tower);
  select_tower(towers.end());
}

void
TowerManager::keydown(const engine::Event& ge)
{
  switch (ge.ev.key.keysym.sym) {
  case SDLK_ESCAPE:
    select_tower(towers.end());
    build_tower_unset();
    break;
  default:
    break;
  }
}

void
TowerManager::mousedown(const engine::Event& ev)
{
  SDL_MouseButtonEvent event = ev.ev.button;

  if (event.button != SDL_BUTTON_LEFT ||
      hud::ButtonBar::instance().in_turf(event.x, event.y)) {
    return;
  }

  click.x = event.x;
  click.z = event.y;
}

bool
TowerManager::tower_purchase_if()
{
  Map *map = Game::instance().map;
  glm::vec2 hl = map->get_highlight();
  if (0.0f >= hl.x || 0.0f >= hl.y) return false;

  glm::vec3 pos = map->get_center_of(hl.x, hl.y);
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
TowerManager::set_tower_infobox()
{
  Tower *t = selected_tower->second;

  towerinfo->clear();
  text::Stream ts;
  ts
    << text::Stream::size(32.0f) << utils::colors::orange << t->get_name() << "\n"
    << text::Stream::size(16.0f) << text::Stream::indent(18.0f)
    << utils::colors::white << "Level: "       << utils::colors::green << t->level  << "\n"
    << utils::colors::white << "Base damage: " << utils::colors::green << t->damage << "\n"
    << utils::colors::white << "Base range: "  << utils::colors::green << t->range  << "\n"
    << utils::colors::white << "Base reload: " << utils::colors::green << t->reload;
  towerinfo->set_content(ts);
}

void
TowerManager::set_sell_infobox()
{
  sellinfo->clear();

  text::Stream ts;
  ts
    << text::Stream::size(24.0f) << utils::colors::red << "Sell tower\n"
    << text::Stream::size(18.0f) << utils::colors::white << "Gold return: "
    << utils::colors::gold << floor(selected_tower->second->price * Player::sell_factor)
    << utils::colors::white << " g";

  sellinfo->set_content(ts);
}

void
TowerManager::set_upgrade_infobox()
{
  upgradeinfo->clear();

  if (0 == upgrades_left(selected_tower)) {
    return;
  }

  Tower *t = selected_tower->second;
  Json::Value upgrade = available_towers
    .find(selected_tower->second->get_name())
    ->second["upgrades"].asArray().at(t->get_level() - 1);

  int delta_dmg      = upgrade["damage"].asInt() - t->damage;
  float delta_range  = round(upgrade["range"].asNumber() - t->range);
  float delta_reload = round(upgrade["reload"].asNumber() - t->reload);

  text::Stream ts;
  ts
    << text::Stream::size(24.0f) << utils::colors::green << "Upgrade tower\n"
    << text::Stream::size(18.0f)
    << utils::colors::white << "Price: "  << utils::colors::gold << upgrade["price"].asInt()
    << utils::colors::white << " g\n"
    << utils::colors::white << "Damage: " << text::Stream::colornumber<int>(delta_dmg,false)      << "\n"
    << utils::colors::white << "Range: "  << text::Stream::colornumber<float>(delta_range,false)  << "\n"
    << utils::colors::white << "Reload: " << text::Stream::colornumber<float>(delta_reload, true);
  upgradeinfo->set_content(ts);
}

void
TowerManager::update_hud()
{
  hud::ButtonBar& bar = hud::ButtonBar::instance();

  if (NULL != btnupgr) {
    btnupgr->disable("leftclick");
    bar.remove_button(btnupgr);
    btnupgr = NULL;
  }

  if (NULL != btnsell) {
    btnsell->disable("leftclick");
    bar.remove_button(btnsell);
    btnsell = NULL;
  }

  if (towers.end() == selected_tower) {
    return;
  }

  std::string tex_upgr = (0 >= upgrades_left(selected_tower)) ?
    TEXTURE_UPGRADE_DISABLED : TEXTURE_UPGRADE;

  btnupgr = new hud::Button(IL::GL::texture(tex_upgr));
  btnsell = new hud::Button(IL::GL::texture(TEXTURE_SELL));

  btnsell->on("leftclick",  std::bind(&TowerManager::sell_tower, this));
  btnsell->on("mouseentry", std::bind(&TowerManager::set_sell_infobox, this));
  btnsell->on("mouseexit",  std::bind(&hud::InfoBox::clear, sellinfo));

  btnupgr->on("leftclick",  std::bind(&TowerManager::upgrade_tower, this));
  btnupgr->on("mouseentry", std::bind(&TowerManager::set_upgrade_infobox, this));
  btnupgr->on("mouseexit",  std::bind(&hud::InfoBox::clear, upgradeinfo));

  bar.add_button(btnupgr, hud::Button::LOCATION_RIGHT);
  bar.add_button(btnsell, hud::Button::LOCATION_RIGHT);
}

void
TowerManager::select_tower(tlist_t::iterator it)
{
  selected_tower = it;

  /* If we do not wish to select a new one, stop here */
  if (selected_tower == towers.end()) {
    towerinfo->clear();
    update_hud();
    return;
  }

  set_tower_infobox();
  update_hud();
}

void
TowerManager::reselect_tower()
{
  auto current = selected_tower;
  select_tower(towers.end());
  select_tower(current);
}

void
TowerManager::tower_select_if(int clickx, int clicky)
{
  try {
    glm::vec3 pos3d = gl::Transform::unproject(clickx, clicky);
    glm::vec3 search = Game::instance().map->get_center_of(pos3d.x, pos3d.z);
    search.y = 0.0f;

    select_tower(towers.find(search));
    build_tower_unset();
  } catch (const Exception& e) {
    /* clicked somewhere not part of map */
    select_tower(towers.end());
  }

  update_hud();
}


void
TowerManager::mouseup(const engine::Event& ge)
{
  SDL_MouseButtonEvent event = ge.ev.button;

  if (hud::ButtonBar::instance().in_turf(event.x, event.y)) return;

  if (event.button == SDL_BUTTON_LEFT &&
      abs(event.x - click.x) < CLICK_FUZZ &&
      abs(event.y - click.z) < CLICK_FUZZ) {

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
TowerManager::tick_nodepth(const engine::Event& ev)
{
  upgradeinfo->draw();
  sellinfo->draw();
  towerinfo->draw();

  for (std::pair<hud::Button*, hud::InfoBox*> entry : browseboxes) {
    entry.second->draw();
  }
}

void
TowerManager::tick(const engine::Event& ev)
{
  float elapsed = ev.elapsed;

  glPushMatrix();
  if (!build_tower.empty() && dummy_tower_pos.length() > 0.0f) {
    /* Currently trying to build a tower */
    dummy_towers.at(build_tower)->draw_range_circle();
  } else if (selected_tower != towers.end()) {
    /* There is a tower selected */
    selected_tower->second->draw_range_circle();
  }
  glPopMatrix();

  for (std::pair<glm::vec3, Tower*> t : towers) {
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
      dummy_tower_shader->use();
      t->draw(elapsed);
      dummy_tower_shader->reset();
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
  }
}

bool
TowerManager::is_building() const
{
  return (!build_tower.empty());
}

bool
TowerManager::has_selected() const
{
  return (towers.end() != selected_tower);
}

bool
TowerManager::escape()
{
  if (is_building()) {
    build_tower_unset();
    return true;
  }

  if (has_selected()) {
    select_tower(towers.end());
    return true;
  }

  return false;
}
