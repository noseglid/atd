#include "Camera.h"
#include "engine/Engine.h"
#include "Debug.h"
#include "text/Dispatcher.h"
#include "gl/Transform.h"

#include <OpenGL/gl.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <float.h>

Camera::Camera() :
  pos(10.0f, 8.0f, 10.0f),
  dir(0.0f, 0.0f, -1.0f),
  up(0.0f, 1.0f, 0.0f),
  mouse_buttons_active(0),
  enabled(true)
{
  set_limits(-FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX);

  engine::Engine& e = engine::Engine::instance();
  e.on("mousedown",   std::bind(&Camera::mousebutton, this, std::placeholders::_1));
  e.on("mouseup",     std::bind(&Camera::mousebutton, this, std::placeholders::_1));
  e.on("mousemotion", std::bind(&Camera::mousemotion, this, std::placeholders::_1));
  e.on("tick_nodepth", std::bind(&Camera::tick, this, std::placeholders::_1));
}

void
Camera::center()
{
  pos.x = (limits.xmin + limits.xmax) / 2.0f;
  pos.y = (limits.ymin + limits.ymax) / 2.0f;
  pos.z = (limits.zmin + limits.zmax) / 2.0f;
  dir.x = -0.87f;
  dir.y = -0.50f;
  dir.z =  0.00f;
}

void
Camera::set_limits(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax)
{
  limits.xmin = xmin;
  limits.xmax = xmax;
  limits.ymin = ymin;
  limits.ymax = ymax;
  limits.zmin = zmin;
  limits.zmax = zmax;
}

void
Camera::set_position(glm::vec3 pos, glm::vec3 dir, glm::vec3 up)
{
  this->pos = pos;
  this->dir = dir;
  this->up  = up;
}

glm::vec3
Camera::get_position() const
{
  return pos;
}

glm::vec3
Camera::get_direction() const
{
  return dir;
}

void
Camera::enable(bool state)
{
  this->enabled = state;
}

void
Camera::impose_limits()
{
  if (pos.x < limits.xmin) pos.x = limits.xmin;
  if (pos.x > limits.xmax) pos.x = limits.xmax;

  if (pos.y < limits.ymin) pos.y = limits.ymin;
  if (pos.y > limits.ymax) pos.y = limits.ymax;

  if (pos.z < limits.zmin) pos.z = limits.zmin;
  if (pos.z > limits.zmax) pos.z = limits.zmax;
}

void
Camera::orientate() const
{
  glm::mat4 matrix = glm::lookAt(pos, pos + dir, up);
  glMultMatrixf(&matrix[0][0]);
}

void
Camera::hover(float dx, float dz)
{
  glm::vec3 perpendicular(dir.z, 0.0f, -dir.x);
  glm::vec3 direction(dir.x, 0.0f, dir.z);

  pos -= perpendicular * dx * pos.y;
  pos -= direction * dz * pos.y;

  impose_limits();
}

void
Camera::mousebutton(const engine::Event& ev)
{
  SDL_MouseButtonEvent event = ev.ev.button;

  mouse_buttons_active ^= event.button;

  if (4 == event.button || 5 == event.button) {
    zoom(0.3 * (4 == event.button ? -1.0f : 1.0f));
  }
}

void
Camera::mousemotion(const engine::Event& ev)
{
  if (!enabled) {
    return;
  }

  SDL_MouseMotionEvent event = ev.ev.motion;

  SDLMod mod = SDL_GetModState();

  if (((SDL_BUTTON_LEFT & mouse_buttons_active) ||
      (SDL_BUTTON_MIDDLE & mouse_buttons_active))
      && !(mod & KMOD_LALT)) {
    hover(event.xrel/60.0f, event.yrel/60.0f);
  }

  if (((SDL_BUTTON_LEFT & mouse_buttons_active) ||
      (SDL_BUTTON_MIDDLE & mouse_buttons_active))
      && (mod & KMOD_LALT)) {
    look(- event.xrel / 5.0f, - event.yrel / 5.0f);
  }
}

void
Camera::tick(const engine::Event& ev)
{
  /*
  std::stringstream ss;
  ss << std::setprecision(2) << std::fixed
     << "Camera position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")";
  gl::Transform::enable2D();
  text::Dispatcher::overlay(ss.str(), 20, 140);
  gl::Transform::disable2D();
  */
}

void
Camera::look(float hangle, float vangle)
{
  glm::mat4 rotate =
    glm::rotate(glm::mat4(1.0), hangle, glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::rotate(glm::mat4(1.0), vangle, glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));

  dir = glm::mat3(rotate) * dir;
}

void
Camera::zoom(float delta)
{
  glm::vec3 newpos = pos - dir * delta;
  if (newpos.y > limits.ymax || newpos.y < limits.ymin) return;

  pos = newpos;
  impose_limits();
}

Camera&
Camera::instance()
{
  static Camera instance;
  return instance;
}
