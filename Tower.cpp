#include "Tower.h"
#include "Text.h"
#include "CreepManager.h"
#include "DummyCreep.h"

#include <OpenGL/gl.h>

static Model *model;

Tower::Tower(Vector3 pos) :
	Purchasable(100),
	pos(pos),
	last_shot(0.0f),
	reload_time(1.2f),
	range(2.0f),
	damage(20.0)
{
}

void
Tower::draw_range_circle() const
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	int points = 20;
	float step =  2*M_PI / (float)points;

	GLfloat
		emission[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
		specular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
		ambient[]  = { 0.0f, 0.0f, 1.0f, 0.1f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambient);

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (int i = points; i >= 0; --i) {
		float x = cos((float)i * step) * range;
		float z = sin((float)i * step) * range;
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	glEnable(GL_DEPTH_TEST);
}

void
Tower::draw(const float& elapsed) const
{
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	model->draw(elapsed);
	//draw_range_circle();
	glPopMatrix();

	for (Projectile *p : projectiles) {
		glPushMatrix();
		p->draw();
		glPopMatrix();
	}
}

Creep *
Tower::get_target()
{
	std::vector<Creep*> creeps = CreepManager::instance().creeps_inside_circle(pos, range);
	if (creeps.empty()) return NULL;

	return creeps.front();
}

void
Tower::update_projectiles(const float& elapsed)
{
	std::list<Projectile *> tmp = projectiles;
	for (Projectile *p : tmp) {
		p->tick();
	}
}

void
Tower::remove_projectile(Projectile *p)
{
	projectiles.remove(p);
	delete p;
}

void
Tower::projectile_hit(Projectile *p, Creep *c, de::Emitter::id_t event)
{
	if (c) c->off(event);
	remove_projectile(p);
}

void
Tower::projectile_notarget(Projectile *p)
{
	DummyCreep *target = new DummyCreep(p->get_target_pos());
	p->set_target(target);
	p->disable("hit");
	p->on("hit", std::bind(&Tower::remove_projectile, this, p));
}

void
Tower::shoot_if(const float& elapsed)
{
	if (elapsed - last_shot < reload_time) return;

	Creep *target = get_target();
	if (NULL == target) return;

	Projectile *p = new Projectile(
		target,
		Vector3(pos.x, pos.y + 1.0f, pos.z),
		damage
	);

	auto death_cb = std::bind(&Tower::projectile_notarget, this, p);
	de::Emitter::id_t death_event = target->on("death", death_cb);

	auto hit_cb = std::bind(&Tower::projectile_hit, this, p, target, death_event);
	p->on("hit", hit_cb);

	projectiles.push_back(p);

	last_shot = elapsed;
}

void
Tower::init()
{
	model = new Model("models/tower/tower.dae");
}
