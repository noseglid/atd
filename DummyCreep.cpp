#include "DummyCreep.h"

DummyCreep::DummyCreep(Vector3 pos) :
	pos(pos)
{
}

Vector3
DummyCreep::get_position() const
{
	return pos;
}

void
DummyCreep::strike(Projectile *p)
{
}
