#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Math.h"

#include <OpenGL/gl.h>

class Camera
{
	Vector3 pos, dir, up;
	GLdouble xzangle;
	int mouse_buttons_active;

public:

	Camera();
	~Camera();

	void orientate() const;
	void hover(GLdouble dx, GLdouble dz);
	void look(GLdouble dx, GLdouble dy);
	void zoom(GLdouble delta);

	void mousebutton();
	void mousemotion();
};

#endif
