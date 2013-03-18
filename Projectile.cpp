#include "Projectile.h"

Projectile::Projectile(Creep *target, Vector3 pos, float damage) :
	target(NULL), pos(pos), damage(damage)
{
	set_target(target);
}

Projectile::~Projectile()
{
}

void
Projectile::set_target(Creep *itarget)
{
  auto creep_death_cb = std::bind(&Projectile::creep_death, this);
	target = itarget;
	if (target) death_event = target->on("death", creep_death_cb);
}

void
Projectile::tick()
{
	if(NULL == target) return;

	Vector3 creep_pos = target->get_position();
	Vector3 dest(creep_pos.x, creep_pos.y + 0.5, creep_pos.z);
	Vector3 dir = dest - pos;
	if (dir.length() < 0.1) {
		target->off(death_event);
		target->alter_health(-damage);

		emit("hit");
		return;
	}

	dir.normalize();
	dir /= 15.0f;
	pos += dir;
}

void
Projectile::creep_death()
{
	set_target(NULL);
	emit("notarget");
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
