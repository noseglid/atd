#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "Targetable.h"
#include "Mobile.h"
#include "gl/glm.h"
#include "Model.h"

#include <de.h>

class Projectile : public de::Emitter<>, public Mobile
{
  Targetable *target;
  glm::vec3 pos, target_pos;
  float max_distance;
  float damage;

  Emitter::id_t death_event;
  Model *model;

public:
  Projectile(Targetable *target, glm::vec3 pos, float damage);
  virtual ~Projectile();
  void set_target(Targetable *target);
  glm::vec3 get_target_pos() const;
  float get_damage() const;

  void tick(const float& elapsed);
  void draw() const;

};

#endif
