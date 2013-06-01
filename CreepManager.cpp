#include "CreepManager.h"
#include "IO.h"
#include "Player.h"
#include "Text.h"
#include "Debug.h"
#include "Game.h"

#define SPAWN_INTERVAL 1.0f
#define COUNT 100

CreepManager&
CreepManager::instance()
{
  static CreepManager instance;
  return instance;
}

CreepManager::CreepManager() : last_spawn(-SPAWN_INTERVAL), spawned(0)
{
  Game::instance().on("tick", std::bind(&CreepManager::tick, this, std::placeholders::_1));
}

void
CreepManager::setup(const Json::Value& levelspec)
{
  Json::Value waves = levelspec["waves"];

  float spawntime = 0;

  for (Json::Value wave : levelspec["waves"].asArray()) {
    wave_t w;
    for (Json::Value creep : wave["creeps"].asArray()) {
      Json::Value spec = Json::deserialize(IO::file_get_contents(creep["spec"].asString()));
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
    Creep *creep = new Creep(spawn.spec);
    creep->on("death", std::bind(&CreepManager::creep_death, this, creep));
    creep->on("accomplished", std::bind(&CreepManager::creep_accomplished, this, creep));
    creeps.push_back(creep);
    wave.pop_front();
  }
}

void
CreepManager::tick(const GameEvent& ev)
{
  float elapsed = ev.elapsed;

  std::list<Creep *> tmp = creeps;
  for (Creep *c : tmp) {
    glPushMatrix();
    c->draw(elapsed);
    glPopMatrix();

    c->tick(elapsed);
  }

  check_spawn(elapsed);
}

void
CreepManager::creep_death(Creep *creep)
{
  Player::instance().alter_gold(creep->reward);

  std::stringstream ss;
  ss << creep->reward << "g";
  Text::set_color(1.0f, 0.9f, 0.0f);
  Text::scrolling(ss.str(), creep->get_position());

  remove_creep(creep);
}

void
CreepManager::creep_accomplished(Creep *creep)
{
  Player::instance().alter_lives(-(creep->life_cost));

  std::stringstream ss;
  ss << "-" << creep->life_cost;
  Text::set_color(1.0f, 1.0f, 1.0f);
  Text::scrolling(ss.str(), creep->get_position());

  remove_creep(creep);
}

void
CreepManager::remove_creep(Creep *creep)
{
  creeps.remove(creep);
  delete creep;
}

std::vector<Creep*>
CreepManager::creeps_inside_circle(Vector3 center, float radius)
{
  std::map<float, Creep*> sorted_result;
  for (Creep *c : creeps) {
    Vector3 delta = c->get_position() - center;
    delta *= delta;
    if (delta.x + delta.z <= radius * radius && 0 < c->get_health()) {
      sorted_result.insert(std::make_pair(c->get_distance_moved(), c));
    }
  }

  std::vector<Creep*> result;
  for (auto it = sorted_result.rbegin(); it != sorted_result.rend(); it++) {
    result.push_back(it->second);
  }

  return result;
}
