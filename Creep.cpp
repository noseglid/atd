#include "Creep.h"
#include "Player.h"
#include "CreepManager.h"
#include "Text.h"
#include "GLTransform.h"

Creep::Creep(Map *map, float health, int reward) :
	total_health(health), current_health(health), reward(reward)
{
	this->map  = map;
	this->path = map->get_path();

	PathCoord start = path->get_start();
	pos = map->get_center_of(start.x, start.y);

	travel_to(path->next_coord(start));
}

Creep::~Creep()
{
}

void
Creep::travel_to(const PathCoord& target)
{
	this->target = target;

	Vector3 target_center = map->get_center_of(target.x, target.y);
	dir = (target_center - pos) / 40.0f;
}

void
Creep::alter_health(float dh)
{
	this->current_health += dh;
	if (this->current_health <= 0.0f) {
		Player::instance().alter_gold(this->reward);

		std::stringstream ss;
		ss << reward << "g";
		Text::scrolling(ss.str(), pos, Vector3(1.0, 0.9, 0.0));

		emit("death");
	}
}

void
Creep::tick(const float& elapsed)
{
	Vector3 tc = map->get_center_of(target.x, target.y);
	if ((dir.x > 0 && pos.x > tc.x) || (dir.x < 0 && pos.x < tc.x) ||
	    (dir.z > 0 && pos.z > tc.z) || (dir.z < 0 && pos.z < tc.z)) {
		travel_to(path->next_coord(target));
	}

	pos += dir;
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
