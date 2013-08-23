#include "Camera.h"
#include "hud/ButtonBar.h"

#include <OpenGL/glu.h>
#include <iostream>
#include <cmath>

Camera::Camera() : mouse_buttons_active(0)
{
  pos.x = 10;
  pos.y = 8;
  pos.z = 10;
  dir.x = 0.00;
  dir.y = -1.00;
  dir.z = -0.70;
  up.x = 0;
  up.y = 1;
  up.z = 0;

  xzangle = asin(dir.x);
  engine::Engine& e = engine::Engine::instance();
  e.on("mousedown",   std::bind(&Camera::mousebutton, this, std::placeholders::_1));
  e.on("mouseup",     std::bind(&Camera::mousebutton, this, std::placeholders::_1));
  e.on("mousemotion", std::bind(&Camera::mousemotion, this, std::placeholders::_1));
}

void
Camera::center()
{
  pos.x = (limits.xmin + limits.xmax) / 2;
  pos.y = 8.0f;
  pos.z = (limits.zmin + limits.zmax) / 2;
  dir.x = 0.00;
  dir.y = -1.00;
  dir.z = -0.70;
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

  center();
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
  gluLookAt(
    pos.x, pos.y, pos.z,
    pos.x + dir.x, pos.y + dir.y, pos.z + dir.z,
    up.x, up.y, up.z
  );
}

void
Camera::hover(GLdouble dx, GLdouble dz)
{
  Vector3 perpendicular(dir.z, 0, -dir.x);
  Vector3 direction(dir.x, 0, dir.z);

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
  if (HUD::ButtonBar::instance().in_turf(ev.ev.motion.x, ev.ev.motion.y)) {
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
    look(deg2rad(event.xrel)/5.0f, -deg2rad(event.yrel)/5.0f);
  }
}

void
Camera::look(GLdouble hangle, GLdouble vangle)
{
  Matrix4 rotationm = Matrix4::rotatey(hangle);
  dir = rotationm * dir;
  dir.normalize();
}

void
Camera::zoom(GLdouble delta)
{
  Vector3 newpos = pos - dir * delta;
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
