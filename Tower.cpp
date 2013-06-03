#include "Tower.h"
#include "GLShapes.h"
#include "Text.h"
#include "CreepManager.h"
#include "DummyCreep.h"

#include <OpenGL/gl.h>

Tower::Tower(Json::Value spec, Vector3 pos) :
  Purchasable(spec["price"].asInt()),
  pos(pos),
  name(spec["name"].asString()),
  level(1),
  reload(spec["reload"].asNumber()),
  range(spec["range"].asNumber()),
  damage(spec["damage"].asNumber()),
  last_shot(0.0f),
  model(Model::load(spec["model"].asString()))
{
}

void
Tower::draw_range_circle() const
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);

  glTranslatef(pos.x, pos.y, pos.z);
  glColor4f(0.0f, 0.0f, 0.8f, 0.2f);
  GLShapes::circle(range);

  glEnable(GL_DEPTH_TEST);
}

void
Tower::draw(const float& elapsed, const float& opacity) const
{
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);
  model->draw(elapsed, opacity);
  glPopMatrix();

  for (Projectile *p : projectiles) {
    glPushMatrix();
    p->draw();
    glPopMatrix();
  }
}

void
Tower::set_position(Vector3 pos)
{
  this->pos = pos;
}

void
Tower::upgrade(int level, float reload, float range, float damage)
{
  this->level  = level;
  this->reload = reload;
  this->range  = range;
  this->damage = damage;
}

Vector3
Tower::get_position() const
{
  return this->pos;
}

std::string
Tower::get_name() const
{
  return name;
}

int
Tower::get_level() const
{
  return level;
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
    p->tick(elapsed);
  }
}

void
Tower::remove_projectile(Projectile *p)
{
  projectiles.remove(p);
  delete p;
}

void
Tower::projectile_hit(
  Projectile *projectile,
  Creep *creep,
  de::Emitter<>::id_t deathev,
  de::Emitter<>::id_t accev)
{
  if (creep) {
    creep->off(deathev);
    creep->off(accev);
  }

  remove_projectile(projectile);
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
  if (last_shot > 0.0f && elapsed - last_shot < reload) return;

  Creep *target = get_target();
  if (NULL == target) return;

  Projectile *p = new Projectile(
    target,
    Vector3(pos.x, pos.y + 1.0f, pos.z),
    damage
  );

  auto creep_gone = std::bind(&Tower::projectile_notarget, this, p);
  de::Emitter<>::id_t deathev = target->on("death", creep_gone);
  de::Emitter<>::id_t accev = target->on("accomplished", creep_gone);

  auto hit_cb = std::bind(&Tower::projectile_hit, this, p, target, deathev, accev);
  p->on("hit", hit_cb);

  projectiles.push_back(p);

  last_shot = elapsed;
}

