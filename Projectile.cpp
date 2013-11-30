#include "Projectile.h"
#include "Debug.h"

#include <OpenGL/gl.h>
#include <cmath>

Projectile::Projectile(Targetable *target, glm::vec3 pos, float damage) :
  Mobile(5.2),
  target(target),
  pos(pos),
  target_pos(target->get_position()),
  damage(damage)
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

glm::vec3
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
Projectile::tick(const float& elapsed)
{
  if(NULL == target) return;

  target_pos = target->get_position();

  glm::vec3 dest(target_pos.x, target_pos.y + 0.5, target_pos.z);
  glm::vec3 dir = glm::normalize(dest - pos);

  glm::vec3 step = dir * get_speed_factor(elapsed);

  /* Derived from P + aS = T, where P is position, S is the step,
   * and T is the target, if a < 1.0 then less than one step
   * is required to reach target -> it's a hit */
  float a = (dest.x - pos.x) / step.x;
  float b = (dest.y - pos.y) / step.y;
  float c = (dest.z - pos.z) / step.z;

  /* isnan may happen if there is no difference along one axis,
   * eg: (0.0 - 0.0) / 0.0, therefore we must check all axises */
  if ((!isnan(a) && a < 1.0) ||
      (!isnan(b) && b < 1.0) ||
      (!isnan(c) && c < 1.0)) {
    target->struck(this);
    emit("hit");
    return;
  }

  pos += step;
}

void
Projectile::draw() const
{
  glDisable(GL_LIGHTING);

  glPointSize(6.0);
  glColor3f(0.0f, 0.0f, 0.0f);

  glTranslatef(pos.x, pos.y, pos.z);
  glBegin(GL_POINTS);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glEnd();
}
