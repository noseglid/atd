#include "Creep.h"
#include "Map.h"
#include "Debug.h"
#include "Exception.h"
#include "Player.h"
#include "CreepManager.h"
#include "Text.h"
#include "GLTransform.h"

Creep::Creep(float health, int reward, int life_cost, float speed) :
	Mobile(speed),
	total_health(health),
	current_health(health),
	reward(reward),
	life_cost(life_cost)
{
	this->path = Map::instance().get_path();

	PathCoord start = path->get_start();
	pos = Map::instance().get_center_of(start.x, start.y);

	travel_to(path->next_coord(start));
}

Creep::~Creep()
{
}

void
Creep::travel_to(const PathCoord& target)
{
	this->target = target;

	vtarget = Map::instance().get_center_of(target.x, target.y);
	dir = (vtarget - pos);
	dir.normalize();
}

void
Creep::struck(Projectile *p)
{
	this->current_health -= p->get_damage();
	if (this->current_health <= 0.0f) {
		emit("death");
	}
}

void
Creep::tick(const float& elapsed)
{
	try {
		float sf = get_speed_factor(elapsed);
		pos += dir * sf;

		/* Will yet another step takes us past our target? */
		if ((dir * sf + pos - vtarget).length() > (pos - vtarget).length()) {
			/* `next_coord` will throw if there are no more coords */
			pos = vtarget;
			travel_to(path->next_coord(target));
		}
	} catch (const NoMoreCoords& e) {
		emit("accomplished");
	}
}

Vector3
Creep::get_position() const
{
	return pos;
}

float
Creep::get_health() const
{
	return current_health;
}

void
Creep::draw(const float& elapsed) const
{
	glTranslatef(pos.x, pos.y + 0.5f, pos.z);

	glPushMatrix();
	this->draw_model(elapsed);
	glPopMatrix();

	glPushMatrix();
	this->draw_health();
	glPopMatrix();

}

void
Creep::draw_health() const
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	GLfloat width = 0.3f;
	GLfloat cur   = (current_health / total_health) * 0.3f;

	GLTransform::billboard();

	glTranslatef(-width / 2.0f, 0.2f, 0.2f);

	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0,    0.03f, 0.0f);
	glVertex3f(0,   -0.03f, 0.0f);
	glVertex3f(cur,  0.03f, 0.0f);
	glVertex3f(cur, -0.03f, 0.0f);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(cur,    0.03f, 0.0f);
	glVertex3f(cur,   -0.03f, 0.0f);
	glVertex3f(width,  0.03f, 0.0f);
	glVertex3f(width, -0.03f, 0.0f);
	glEnd();

	glEnable(GL_LIGHTING);
}
