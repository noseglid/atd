#include "Camera.h"
#include "Game.h"

#include <OpenGL/glu.h>
#include <iostream>
#include <cmath>

Camera::Camera() : mouse_buttons_active(0)
{
	pos.x = 1;
	pos.y = 8;
	pos.z = 1;
	dir.x = 0.50;
	dir.y = -1.00;
	dir.z = 0.51;
	up.x = 0;
	up.y = 1;
	up.z = 0;

	xzangle = asin(dir.x);
	Game& g = Game::instance();
	g.on("mousedown", std::bind(&Camera::mousebutton, this, std::placeholders::_1));
	g.on("mouseup", std::bind(&Camera::mousebutton, this, std::placeholders::_1));
	g.on("mousemotion", std::bind(&Camera::mousemotion, this, std::placeholders::_1));
}

Camera::~Camera()
{
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

	if (pos.x < limits.xmin) pos.x = limits.xmin;
	if (pos.x > limits.xmax) pos.x = limits.xmax;
	if (pos.z < limits.zmin) pos.z = limits.zmin;
	if (pos.z > limits.zmax) pos.z = limits.zmax;
}

void
Camera::mousebutton(const GameEvent& ev)
{
	SDL_MouseButtonEvent event = ev.ev.button;

	mouse_buttons_active ^= event.button;

	if (4 == event.button || 5 == event.button) {
		zoom(0.3 * (4 == event.button ? -1.0f : 1.0f));
	}
}

void
Camera::mousemotion(const GameEvent& ev)
{
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
	if (pos.y < limits.ymin && delta < 0) return;
	if (pos.y > limits.ymax && delta > 0) return;

	pos -= dir * delta;
}

Camera&
Camera::instance()
{
	static Camera instance;
	return instance;
}
