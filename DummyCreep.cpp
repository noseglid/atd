#include "DummyCreep.h"

DummyCreep::DummyCreep(glm::vec3 pos) :
  pos(pos)
{
}

DummyCreep::~DummyCreep()
{
}

glm::vec3
DummyCreep::get_position() const
{
  return pos;
}

void
DummyCreep::struck(Projectile *p)
{
}
