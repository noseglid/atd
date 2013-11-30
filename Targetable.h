#ifndef __TARGETABLE_H__
#define __TARGETABLE_H__

#include "gl/glm.h"

class Projectile;

class Targetable
{
public:
  virtual glm::vec3 get_position() const = 0;
  virtual void struck(Projectile *p) = 0;
};

#endif
