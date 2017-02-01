#include "Projectile.h"
#include "engine/Engine.h"
#include "Debug.h"

#include <OpenGL/gl.h>
#include <cmath>

Projectile::Projectile(Targetable *target, glm::vec3 pos, float damage) :
  Mobile(4.0f),
  pos(pos),
  max_distance(-1.0f),
  damage(damage),
  model(Model::load("resources/models/projectiles/bolt.dae"))
{
  set_target(target);
}

Projectile::~Projectile()
{
}

void
Projectile::set_target(Targetable *itarget)
{
  target = itarget;
  target_pos  = target->get_position();
  target_pos += glm::vec3(0.0f, 0.5f, 0.0f); // XXX: this should probably be creep boundingbox
  max_distance = glm::distance(target_pos, pos);
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
  target_pos += glm::vec3(0.0f, 0.5f, 0.0f); // XXX: this should probably be creep boundingbox

  glm::vec3 dir  = glm::normalize(target_pos - pos);
  glm::vec3 step = dir * get_speed_factor(elapsed);

  /* Derived from P + aS = T, where P is position, S is the step,
   * and T is the target, if a < 1.0 then less than one step
   * is required to reach target -> it's a hit */
  float a = (target_pos.x - pos.x) / step.x;
  float b = (target_pos.y - pos.y) / step.y;
  float c = (target_pos.z - pos.z) / step.z;

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

static float
trajectory(float x)
{
  /* Cubic parabola from points (0.0, 0.0), (0.5, 1.0) and (1.0, 0.0) */
  return 4.0 * x - 4.0 * x * x;
}

static float
trajectory_slope(float x)
{
  /* Derivative of `trajectory` */
  return 4.0f - 8.0 * x;
}

void
Projectile::draw() const
{
  glm::vec3 dir = target_pos - pos;

  /* Normalized distance, [0.0 .. 1.0] */
  float distance = 1.0 - glm::distance(target_pos, pos) / max_distance;
  if (0 >= distance) return;

  float height = trajectory(distance);

  float isect = height - trajectory_slope(distance) * distance;
  glm::vec2 xzvec = glm::vec2(0.0f, isect) - glm::vec2(distance, height);

  float yrotation = glm::orientedAngle(
    glm::normalize(glm::vec2(dir.x, dir.z)),
    glm::vec2(0.0f, 1.0f)
  );

  float xrotation = glm::orientedAngle(
    glm::normalize(xzvec),
    glm::vec2(-1.0f, 0.0f)
  );

  height += (1.0f - distance); // XXX: this should probably be the height of tower boundingbox
  glm::mat4 matrix =
    glm::translate(glm::mat4(1.0), glm::vec3(pos.x, pos.y + height, pos.z)) *
    glm::rotate(glm::mat4(1.0f), yrotation, glm::vec3(0.0, 1.0, 0.0)) *
    glm::rotate(glm::mat4(1.0f), xrotation, glm::vec3(1.0, 0.0, 0.0));

  glPushMatrix();
    glMultMatrixf(&matrix[0][0]);
    model->draw(engine::Engine::instance().get_elapsed());
  glPopMatrix();
}
