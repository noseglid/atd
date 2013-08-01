#ifndef __MAP_H__
#define __MAP_H__

#include "engine/Engine.h"
#include "Path.h"
#include "Model.h"
#include "math/Math.h"

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
  typedef std::vector<std::vector<float>> heightmap_t;
  typedef std::vector<std::vector<Vector3>> normals_t;
  typedef struct {
    Model *model;
    float tx, ty, tz;
    float angle, rx, ry, rz;
    float sx, sy, sz;
  } scenery_t;

  size_t width, height;
  Vector2 highlighted;
  bool draw_meta;

  Path *path;
  GLint cliff_texture;
  std::vector<GLint> textures;

  heightmap_t heightmap;
  Vector3 **normals;

  size_t edge_width;
  heightmap_t heightmap_edge;
  normals_t normals_edge;

  std::vector<scenery_t> scenery;

  void load_textures(const Json::Value& v);

  void create_map_heightmap();
  void create_edge_heightmap();

  Vector3 calc_normal(
    int h, int hmax,
    int w, int wmax,
    const heightmap_t& map,
    float exaggeration = 1.0f) const;

  void generate_map_normals();
  void generate_edge_normals();

  void draw_edge_wall() const;
  void draw_scenery(const float& elapsed) const;
  void draw_square(const int&x, const int& y) const;

public:
  Map();
  ~Map();

  void load(const Json::Value& levelspec);
  void tick(const engine::Event& ge);
  void mousemotion(const engine::Event& ge);
  void keydown(const engine::Event& ge);

  void draw(const float& elapsed) const;
  void draw_normals() const;
  Vector3 get_center_of(int x, int y) const;
  const Path *get_path() const;
  size_t get_width() const;
  size_t get_height() const;

  void set_highlight(const int& x, const int& y);
  Vector2 get_highlight() const;

  static Map& instance();
};

#endif
