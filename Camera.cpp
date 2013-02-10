#include "Camera.h"
#include "Game.h"
#include "Math.h"

#include <OpenGL/glu.h>
#include <iostream>
#include <cmath>

#define MAX_HEIGHT 8
#define MIN_HEIGHT 1

Camera::Camera() : mouse_buttons_active(0)
{
	pos.x = 5;
	pos.y = 8;
	pos.z = 18;
	dir.x = 0;
	dir.y = -0.71;
	dir.z = -0.51;
	up.x = 0;
	up.y = 1;
	up.z = 0;

	xzangle = asin(dir.x);
	Game::instance().register_listener(this)
		<< SDL_MOUSEBUTTONDOWN
		<< SDL_MOUSEBUTTONUP
		<< SDL_MOUSEMOTION;
}

Camera::~Camera()
{
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
}

void
Camera::event(const SDL_MouseButtonEvent& event)
{
	mouse_buttons_active ^= event.button;

	if (4 == event.button || 5 == event.button) {
		zoom(0.3 * (4 == event.button ? -1.0f : 1.0f));
	}
}

void
Camera::event(const SDL_MouseMotionEvent& event)
{
	SDLMod mod = SDL_GetModState();

	if ((SDL_BUTTON_LEFT & mouse_buttons_active) && !(mod & KMOD_LCTRL)) {
		hover(event.xrel/60.0f, event.yrel/60.0f);
	}

	if ((SDL_BUTTON_LEFT & mouse_buttons_active) && (mod & KMOD_LCTRL)) {
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
	if ((pos.y < MIN_HEIGHT && delta < 0) || (pos.y > MAX_HEIGHT && delta > 0)) return;
	pos -= dir * delta;
}
