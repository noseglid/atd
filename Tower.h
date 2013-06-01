#ifndef __TOWER_H__
#define __TOWER_H__

#include "math/Math.h"
#include "Model.h"
#include "Creep.h"
#include "Projectile.h"
#include "Purchasable.h"

#include <de.h>
#include <pjson.hpp>
#include <list>

class Tower : public Purchasable
{
  Vector3 pos;
  std::list<Projectile *> projectiles;

  Creep *get_target();

  std::string name;
  int level;
  float reload, range, damage, last_shot;
  Model *model;

public:
  Tower(Json::Value spec, Vector3 pos);
  void draw(const float& elapsed) const;
  void draw_range_circle() const;

  void set_position(Vector3 pos);
  void upgrade(int level, float reload, float range, float damage);

  Vector3 get_position() const;
  std::string get_name() const;
  int get_level() const;

  void remove_projectile(Projectile *p);
  void projectile_hit(Projectile *projectile, Creep *creep,
    de::Emitter<>::id_t deathev, de::Emitter<>::id_t accev);
  void projectile_notarget(Projectile *p);
  void update_projectiles(const float& elapsed);
  void shoot_if(const float& elapsed);
};

#endif
