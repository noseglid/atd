#ifndef __CREEP_H__
#define __CREEP_H__

#include "math/Math.h"
#include "Path.h"
#include "Model.h"
#include "Targetable.h"
#include "Projectile.h"
#include "Mobile.h"

#include <de.h>
#include <pjson.hpp>
#include <SDL/SDL_mixer.h>

class Creep : public de::Emitter<>, public Targetable, public Mobile
{
  friend class CreepManager;

  void travel_to(const PathCoord& target);

  Mix_Chunk *audio_death;
  Model *model;

protected:
  const Path *path;

  PathCoord target;
  Vector3 vtarget;

  float total_health, current_health;
  int reward;
  int life_cost;

  Vector3 pos, dir;

public:
  Creep(Json::Value spec);
  virtual ~Creep();

  virtual void struck(Projectile *p);
  virtual Vector3 get_position() const;

  float get_health() const;

  void draw(const float& elapsed) const;
  void draw_health() const;
  void tick(const float& elapsed);
};

#endif
