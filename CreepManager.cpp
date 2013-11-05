#include "CreepManager.h"
#include "hud/InfoBox.h"
#include "IO.h"
#include "Game.h"
#include "Player.h"
#include "text/Text.h"
#include "text/Dispatcher.h"
#include "Debug.h"

CreepManager::CreepManager(const Json::Value& levelspec) :
  starttime(-1.0f), last_spawn(0), spawned(0)
{
  DBG("Registering events for CreepManager");
  tickev = engine::Engine::instance().on(
    "tick", std::bind(&CreepManager::tick, this, std::placeholders::_1)
  );

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
  waves_total = waves.asArray().size();

  float spawntime = 0;

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

  stats_text();
}

void
CreepManager::stats_text() const
{
  text::Stream ts;
  int waves_left  = (0 == spawns.size()) ? 0 : spawns.size() - 1;
  int creeps_left = (0 == spawns.size()) ? 0 : spawns.front().size();
  ts << text::Stream::size(20.0f)
     << utils::colors::white << "Current wave: " << (waves_total - waves_left) << "\n"
     << utils::colors::white << "Creeps left: " << utils::colors::green << creeps_left << "\n"
     << utils::colors::white << "Waves left: "  << utils::colors::green << waves_left;
  HUD::InfoBar::instance().set_info_text2(ts);
}

void
CreepManager::check_spawn(const float& elapsed)
{
  if (0 > starttime) starttime = engine::Engine::instance().get_elapsed();

  if (spawns.empty()) {
    /* All waves complete. When last creeps either dies or accomplishes, it's done */
    return;
  }

  wave_t& wave = spawns.front();
  if (wave.empty()) {
    /* Wave complete */
    spawns.pop_front();
    stats_text();
    return;
  }

  spawn_t& spawn = wave.front();
  if (elapsed >= starttime + spawn.time) {
    Creep *creep = new Creep(spawn.spec, 10.0f * rand() / RAND_MAX );
    creep->on("death", std::bind(&CreepManager::creep_death, this, creep));
    creep->on("accomplished", std::bind(&CreepManager::creep_accomplished, this, creep));
    creeps.push_back(creep);
    wave.pop_front();
    stats_text();
  }
}

void
CreepManager::tick(const engine::Event& ev)
{
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
  text::Dispatcher::instance().scrolling(ss.str(), creep->get_position(), utils::colors::gold);

  remove_creep(creep);
}

void
CreepManager::creep_accomplished(Creep *creep)
{
  Game::instance().player->alter_lives(-(creep->life_cost));

  std::stringstream ss;
  ss << "-" << creep->life_cost;
  text::Dispatcher::instance().scrolling(ss.str(), creep->get_position());

  remove_creep(creep);
}

void
CreepManager::remove_creep(Creep *creep)
{
  creeps.remove(creep);
  delete creep;

  if (level_complete() && Game::instance().player->is_alive()) {
    emit("complete");
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
