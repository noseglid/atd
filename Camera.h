#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "engine/Event.h"
#include "gl/glm.h"

class Camera
{
  glm::vec3 pos, dir, up;
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
    glm::vec3 pos = glm::vec3(0.0f, 0.0f,  2.5f),
    glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3 up  = glm::vec3(0.0f, 1.0f,  0.0f)
  );
  glm::vec3 get_direction() const;
  void enable(bool state);

  void orientate() const;
  void hover(float dx, float dz);
  void look(float dx, float dy);
  void zoom(float delta);

  void mousebutton(const engine::Event& ev);
  void mousemotion(const engine::Event& ev);
  void tick(const engine::Event& ev);

  static Camera& instance();
};

#endif
