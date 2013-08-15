#include "CreepManager.h"
#include "hud/InfoBox.h"
#include "IO.h"
#include "Game.h"
#include "Player.h"
#include "Text.h"
#include "Debug.h"

CreepManager::CreepManager(const Json::Value& levelspec) :
  last_spawn(0), spawned(0), winbox(HUD::InfoBox::SNAP_CENTER)
{
  DBG("Registering events for CreepManager");
  tickev = engine::Engine::instance().on(
    "tick", std::bind(&CreepManager::tick, this, std::placeholders::_1)
  );

  using HUD::InfoBox;
  winbox << InfoBox::size(32.0f) << utils::colors::green << "Level complete!";

  setup(levelspec);
}

CreepManager::~CreepManager()
{
  DBG("Deregistering CreepManager from events");
  engine::Engine::instance().off(tickev);
}

void
CreepManager::setup(const Json::Value& levelspec)
{
  Json::Value waves = levelspec["waves"];

  float spawntime = engine::Engine::instance().get_elapsed();

  for (Json::Value wave : levelspec["waves"].asArray()) {
    wave_t w;
    for (Json::Value creep : wave["creeps"].asArray()) {
      std::string specfile = "resources/specs/creeps/" + creep["spec"].asString() + ".json";
      Json::Value spec = Json::deserialize(IO::file_get_contents(specfile));
      for (int i = 0; i < creep["count"].asNumber(); ++i) {
        spawn_t s;
        s.spec = spec;
        spawntime += creep["spawninterval"].asNumber();
        s.time = spawntime;
        w.push_back(s);
      }
    }
    spawns.push_back(w);
  }
}

void
CreepManager::check_spawn(const float& elapsed)
{
  if (spawns.empty()) {
    /* All waves complete. When last creeps either dies or accomplishes, it's done */
    return;
  }

  wave_t& wave = spawns.front();
  if (wave.empty()) {
    /* Wave complete */
    spawns.pop_front();
    return;
  }

  spawn_t& spawn = wave.front();
  if (elapsed >= spawn.time) {
    Creep *creep = new Creep(spawn.spec, 10.0f * rand() / RAND_MAX );
    creep->on("death", std::bind(&CreepManager::creep_death, this, creep));
    creep->on("accomplished", std::bind(&CreepManager::creep_accomplished, this, creep));
    creeps.push_back(creep);
    wave.pop_front();
  }
}

void
CreepManager::tick(const engine::Event& ev)
{
  if (level_complete()) {
    winbox.draw();
  }

  std::list<Creep *> tmp = creeps;
  for (Creep *c : tmp) {
    glPushMatrix();
    c->draw(ev.elapsed);
    glPopMatrix();

    c->tick(ev.elapsed);
  }

  check_spawn(ev.elapsed);
}

bool
CreepManager::level_complete() const
{
  return spawns.empty() && creeps.empty();
}

void
CreepManager::creep_death(Creep *creep)
{
  Game::instance().player->alter_gold(creep->reward);

  std::stringstream ss;
  ss << "+" << creep->reward << "g";
  Text::set_color(utils::colors::gold);
  Text::scrolling(ss.str(), creep->get_position());

  remove_creep(creep);
}

void
CreepManager::creep_accomplished(Creep *creep)
{
  Game::instance().player->alter_lives(-(creep->life_cost));

  std::stringstream ss;
  ss << "-" << creep->life_cost;
  Text::set_color(utils::colors::white);
  Text::scrolling(ss.str(), creep->get_position());

  remove_creep(creep);
}

void
CreepManager::remove_creep(Creep *creep)
{
  creeps.remove(creep);
  delete creep;

  if (level_complete()) {
    engine::Engine::instance().queue_event(7.0f, []() {
      Game::instance().stop();
    });
  }
}

std::vector<Creep*>
CreepManager::creeps_inside_circle(Vector3 center, float radius)
{
  std::map<float, Creep*> sorted_result;
  float radius_squared = radius * radius;
  for (Creep *c : creeps) {
    Vector3 delta = c->get_position() - center;
    delta *= delta;
    if (delta.x + delta.z <= radius_squared) {
      sorted_result.insert(std::make_pair(c->get_distance_moved(), c));
    }
  }

  std::vector<Creep*> result;
  for (auto it = sorted_result.rbegin(); it != sorted_result.rend(); it++) {
    result.push_back(it->second);
  }

  return result;
}
