#include "Creep.h"
#include "Audio.h"
#include "Game.h"
#include "Map.h"
#include "Debug.h"
#include "Exception.h"
#include "Player.h"
#include "CreepManager.h"
#include "GLTransform.h"

Creep::Creep(Json::Value spec, float animinc) :
  Mobile(spec["speed"].asNumber()),
  distance_moved(0),
  audio_death(Audio::instance().load_sfx(spec["audio"]["death"].asString())),
  model(NULL),
  animinc(animinc),
  total_health(spec["health"].asInt()),
  current_health(spec["health"].asInt()),
  reward(spec["reward"].asInt()),
  life_cost(spec["cost"].asInt())
{
  std::string modelfile = "resources/models/" + spec["model"]["resource"].asString() +
    "/" + spec["model"]["resource"].asString() + ".dae";
  model = Model::load(modelfile);

  Map *map = Game::instance().map;
  this->path = map->get_path();
  PathCoord start = path->get_start();
  pos = map->get_center_of(start.x, start.y);
  travel_to(path->next_coord(start));

  trafo = glm::mat4(
    spec["model"]["scale"]["x"].asNumber(), 0.0, 0.0, 0.0,
    0.0, spec["model"]["scale"]["y"].asNumber(), 0.0, 0.0,
    0.0, 0.0, spec["model"]["scale"]["z"].asNumber(), 0.0,
    0.0, 0.0, 0.0, 1.0
  );
}

Creep::~Creep()
{
}

void
Creep::travel_to(const PathCoord& target)
{
  this->target = target;

  vtarget  = Game::instance().map->get_center_of(target.x, target.y);
  dir      = glm::normalize(vtarget - pos);
  rotation = glm::orientedAngle(glm::vec3(0.0f, 0.0f, 1.0f), dir, glm::vec3(0.0f, 1.0f, 0.0f));

}

void
Creep::struck(Projectile *p)
{
  this->current_health -= p->get_damage();
  if (this->current_health <= 0.0f) {
    Audio::instance().play(audio_death);
    emit("death");
  }
}

void
Creep::tick(const float& elapsed)
{
  try {
    float sf = get_speed_factor(elapsed);
    glm::vec3 v = dir * sf;
    pos += v;

    distance_moved += glm::length(v);

    /* Will yet another step takes us past our target? */
    if (glm::length(dir * sf + pos - vtarget) > glm::length(pos - vtarget)) {
      /* `next_coord` will throw if there are no more coords */
      pos = vtarget;
      travel_to(path->next_coord(target));
    }
  } catch (const NoMoreCoords& e) {
    emit("accomplished");
  }
}

glm::vec3
Creep::get_position() const
{
  return pos;
}

float
Creep::get_health() const
{
  return current_health;
}

float
Creep::get_distance_moved() const
{
  return distance_moved;
}

void
Creep::draw(const float& elapsed) const
{
  glTranslatef(pos.x, pos.y + 0.5f, pos.z);
  glRotatef(rotation, 0.0f, 1.0f, 0.0f);

  glPushMatrix();
  glMultMatrixf(&trafo[0][0]);
  this->model->draw(elapsed + animinc);
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
