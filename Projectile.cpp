#include "Projectile.h"
#include "Debug.h"

#include <OpenGL/gl.h>

Projectile::Projectile(Targetable *target, Vector3 pos, float damage) :
	target(target), pos(pos), target_pos(target->get_position()), damage(damage)
{
}

Projectile::~Projectile()
{
}

void
Projectile::set_target(Targetable *itarget)
{
	target = itarget;
}

Vector3
Projectile::get_target_pos() const
{
	return target_pos;
}

float
Projectile::get_damage() const
{
	return damage;
}

void
Projectile::tick()
{
	if(NULL == target) return;

	target_pos = target->get_position();
	if (target_pos.length() < 1) {
		DBG("Found it: " << target_pos);
	}

	Vector3 dest(target_pos.x, target_pos.y + 0.5, target_pos.z);
	Vector3 dir = dest - pos;
	if (dir.length() < 0.1) {
		emit("hit");
		target->strike(this);
		return;
	}

	dir.normalize();
	dir /= 8.0f;
	pos += dir;
}

void
Projectile::draw() const
{
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_LIGHTING);

	glPointSize(6.0);
	glColor3f(0.0f, 0.0f, 0.0f);

	glTranslatef(pos.x, pos.y, pos.z);
	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
}
