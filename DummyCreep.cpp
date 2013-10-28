#include "DummyCreep.h"

DummyCreep::DummyCreep(Vector3 pos) :
  pos(pos)
{
}

DummyCreep::~DummyCreep()
{
}

Vector3
DummyCreep::get_position() const
{
  return pos;
}

void
DummyCreep::struck(Projectile *p)
{
}
