#ifndef __MAP_H__
#define __MAP_H__

#include "engine/Engine.h"
#include "Path.h"
#include "Model.h"
#include "gl/VBO.h"
#include "gl/glm.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <OpenGL/GL.h>

#include <pjson.hpp>
#include <de.h>

struct MapEvent
{
  struct {
    int x, y;
  } hovered;
};

class Map : public de::Emitter<MapEvent>
{
  typedef struct {
    Model *model;
    float tx, ty, tz;
    float angle, rx, ry, rz;
    float sx, sy, sz;
  } scenery_t;

  gl::VBO *vbo;
  std::vector<engine::Engine::id_t> events;

  size_t width, height;
  glm::vec2 highlighted;

  Path *path;

  std::vector<scenery_t> scenery;

  Map(const Map& rhs);
  void operator=(const Map& rhs);

  void load_level(const Json::Value& levelspec);
  void load_textures(const Json::Value& v);
  void generate_map();

public:
  Map(const Json::Value& levelspec);
  ~Map();

  void tick(const engine::Event& ge);
  void mousemotion(const engine::Event& ge);

  void draw(const float& elapsed) const;
  glm::vec3 get_center_of(int x, int y) const;
  const Path *get_path() const;
  size_t get_width() const;
  size_t get_height() const;

  void set_highlight(const int& x, const int& y);
  glm::vec2 get_highlight() const;
};

#endif
