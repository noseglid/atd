#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "math/Math.h"
#include "engine/Event.h"

#include <OpenGL/gl.h>

class Camera
{
  Vector3 pos, dir, up;
  GLdouble xzangle;
  int mouse_buttons_active;
  bool enabled;

  struct {
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
  } limits;

  Camera();
  void center();

  void impose_limits();

public:

  void set_limits(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
  void set_position(
    Vector3 pos = Vector3(0.0f, 0.0f,  2.5f),
    Vector3 dir = Vector3(0.0f, 0.0f, -1.0f),
    Vector3 up  = Vector3(0.0f, 1.0f,  0.0f)
  );
  void enable(bool state);

  void orientate() const;
  void hover(GLdouble dx, GLdouble dz);
  void look(GLdouble dx, GLdouble dy);
  void zoom(GLdouble delta);

  void mousebutton(const engine::Event& ev);
  void mousemotion(const engine::Event& ev);

  static Camera& instance();
};

#endif
