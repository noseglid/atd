#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/Math.h"

#include <OpenGL/gl.h>

struct GameEvent;

class Camera
{
	Vector3 pos, dir, up;
	GLdouble xzangle;
	int mouse_buttons_active;

	struct {
		float xmin, xmax;
		float ymin, ymax;
		float zmin, zmax;
	} limits;


public:

	Camera();
	~Camera();

	void set_limits(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

	void orientate() const;
	void hover(GLdouble dx, GLdouble dz);
	void look(GLdouble dx, GLdouble dy);
	void zoom(GLdouble delta);

	void mousebutton(const GameEvent& ev);
	void mousemotion(const GameEvent& ev);
};

#endif
