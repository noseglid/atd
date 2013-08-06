#ifndef __CREEPMANAGER_H__
#define __CREEPMANAGER_H__

#include "Creep.h"
#include "engine/Engine.h"
#include "hud/InfoBox.h"

#include <pjson.hpp>
#include <list>

class CreepManager
{
  typedef struct {
    Json::Value spec;
    float time;
  } spawn_t;
  typedef std::list<spawn_t> wave_t;
  std::list<wave_t> spawns;

  float last_spawn;
  unsigned int spawned;
  std::list<Creep*> creeps;

  HUD::InfoBox winbox;

  CreepManager();
  void operator=(CreepManager const&);
  CreepManager(const CreepManager&);

  void check_spawn(const float& elapsed);
  void remove_creep(Creep *creep);
  void tick(const engine::Event& ev);

  bool level_complete() const;
  void creep_death(Creep *creep);
  void creep_accomplished(Creep *creep);

public:
  void setup(const Json::Value& levelspec);

  std::vector<Creep*> creeps_inside_circle(Vector3 center, float radius);

  static CreepManager& instance();
};

#endif
