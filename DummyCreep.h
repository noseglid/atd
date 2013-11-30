#ifndef __DUMMYCREEP_H__
#define __DUMMYCREEP_H__

#include "Targetable.h"
#include "gl/glm.h"

class DummyCreep : public Targetable
{
  glm::vec3 pos;

public:
  DummyCreep(glm::vec3 pos);
  virtual ~DummyCreep();

  glm::vec3 get_position() const;
  void struck(Projectile *p);
};

#endif

