#include "Map.h"
#include "Game.h"
#include "HUD.h"
#include "GLTransform.h"
#include "ImageLoader.h"
#include "Exception.h"

#include <iostream>
#include <iterator>
#include <OpenGL/gl.h>
#include <pjson.hpp>
#include <algorithm>
#include <array>

#define SQSIZE 1.0f /* ALTERING NOT SUPPORTED YET */

#define EDGE_STEPS 8

Map::Map()
{
  draw_meta = false;
  highlighted.x = -1;
  highlighted.y = -1;

  Game& g = Game::instance();
  g.on("tick", std::bind(&Map::tick, this, std::placeholders::_1));
  g.on("mousemotion", std::bind(&Map::mousemotion, this, std::placeholders::_1));
  g.on("keydown", std::bind(&Map::keydown, this, std::placeholders::_1));

  cliff_texture = IL::GL::texture("cliff1.jpg");
}

Map::~Map()
{
  for (size_t h = 0; h < height; ++h) {
    delete[] normals[h];
  }
  delete[] normals;

  delete path;
}

void
Map::load(const Json::Value& levelspec)
{
  /* Load metadata */
  width  = levelspec["width"].asInt();
  height = levelspec["height"].asInt();

  /* Load the textures */
  for (Json::Value entry : levelspec["textures"].asArray()) {
    textures.push_back(IL::GL::texture(entry.asString()));
  }

  /* Load the paths */
  Json::Array paths = levelspec["paths"].asArray();
  if (paths.size() != 1) {
    throw Exception("Only one path per levelspec currently supported.");
  }
  Json::Value p = paths[0];
  path = new Path(p["texture"].asInt(), p["path"].asString());

  /* Load the scenery */
  if (levelspec.objectHasKey("scenery")) {
    for (Json::Value entry : levelspec["scenery"].asArray()) {
      scenery_t s;
      s.model = Model::load(entry["model"].asString());
      s.tx    = entry["position"]["x"].asNumber();
      s.ty    = entry["position"]["y"].asNumber();
      s.tz    = entry["position"]["z"].asNumber();
      s.angle = entry["rotation"]["angle"].asNumber();
      s.rx    = entry["rotation"]["x"].asNumber();
      s.ry    = entry["rotation"]["y"].asNumber();
      s.rz    = entry["rotation"]["z"].asNumber();
      s.sx    = entry["scale"]["x"].asNumber();
      s.sy    = entry["scale"]["y"].asNumber();
      s.sz    = entry["scale"]["z"].asNumber();
      scenery.push_back(s);
    }
  }

  create_map_heightmap();
  create_edge_heightmap();
  generate_map_normals();
  generate_edge_normals();
}

void
Map::create_map_heightmap()
{
  for (size_t h = 0; h < height + 2; ++h) {
    std::vector<float> row;
    for (size_t w = 0; w < width + 2; ++w) {
      row.push_back((rand() / (float)RAND_MAX) / 5.0f);
    }
    heightmap.push_back(row);
  }
}

void
Map::create_edge_heightmap()
{
  edge_width = std::max(width, height);
  edge_width += ceil(edge_width / 2);

  for (size_t h = 0; h < EDGE_STEPS + 1; ++h) {
    std::vector<float> row;
    for (size_t w = 0; w < edge_width + 1; ++w) {
      float m = (h - 1.0f) + (rand() / (float)RAND_MAX) * 3.0f;
      m = (h == 0) ? 0 : m;
      m = (h == 1) ? 0.5 : m;
      row.push_back(m);
    }
    heightmap_edge.push_back(row);
  }
}

Vector3
Map::calc_normal(
  int h, int hmax,
  int w, int wmax,
  const heightmap_t& hmap,
  float exaggeration) const
{
  if (0 >= h || hmax - 1 < h || 0 >= w || wmax - 1 < w)
    return Vector3(0.0f, 1.0f, 0.0f);

  Vector3 v1(0,       hmap[h - 1][w], SQSIZE);
  Vector3 v2(SQSIZE,  hmap[h][w + 1], 0);
  Vector3 v3(0,       hmap[h + 1][w], -SQSIZE);
  Vector3 v4(-SQSIZE, hmap[h][w - 1], 0);

  Vector3 res = v1 * v2 + v2 * v3 + v3 * v4 + v4 * v1;
  res.x *= exaggeration;
  res.z *= exaggeration;
  res.normalize();

  return res;
}

void
Map::generate_map_normals()
{
  normals = new Vector3*[height + 2];
  for (size_t h = 0; h < height + 1; ++h) {
    normals[h] = new Vector3[width + 2];
    for (size_t w = 0; w < width + 1; ++w) {
      normals[h][w] = calc_normal(h, height, w, width, heightmap, 4.0f);
    }
  }
}

void
Map::generate_edge_normals()
{
  for (size_t h = 0; h < EDGE_STEPS + 1; ++h) {
    std::vector<Vector3> row;
    for(size_t w = 0; w < edge_width; ++w) {
      row.push_back(calc_normal(h, EDGE_STEPS, w, edge_width, heightmap_edge));
    }
    normals_edge.push_back(row);
  }
}

void
Map::tick(const GameEvent& ge)
{
  this->draw(ge.elapsed);
  if (draw_meta) this->draw_normals();
}

void
Map::keydown(const GameEvent& ge)
{
  SDL_KeyboardEvent event = ge.ev.key;
  switch (event.keysym.sym) {
    case SDLK_m: draw_meta = !draw_meta; break;
    default: break;
  }
}

void
Map::mousemotion(const GameEvent& ge)
{
  static MapEvent me;

  Vector3 v = GLTransform::unproject(ge.ev.motion.x, ge.ev.motion.y);
  int newx = static_cast<int>(v.x);
  int newy = static_cast<int>(v.z);
  if ((newx >= 0 && newx < width && newy >= 0 && newy < height) &&
      (me.hovered.x != newx || me.hovered.y != newy)) {
    me.hovered.x = v.x;
    me.hovered.y = v.z;
    emit("hover", me);
  }
}

void
Map::draw_edge_wall() const
{
  glTranslatef((edge_width - width) / -2.0f, 0.0f, 0.0f);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, cliff_texture);

  static const GLfloat texcoords[4][2] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f }
  };

  glColor3f(0.7f, 0.7f, 0.7f);
  for (int i = 0; i < EDGE_STEPS; ++i) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j < edge_width; ++j) {
      glNormal3fv((GLfloat*)&(normals_edge[i + 1][j]));
      glTexCoord2fv(texcoords[0]);
      glVertex3f(j, heightmap_edge[i + 1][j], -i);

      glNormal3fv((GLfloat*)&(normals_edge[i][j]));
      glTexCoord2fv(texcoords[1]);
      glVertex3f(j, heightmap_edge[i][j], -i + 1.0f);

      glNormal3fv((GLfloat*)&(normals_edge[i + 1][j + 1]));
      glTexCoord2fv(texcoords[2]);
      glVertex3f(j + 1.0f, heightmap_edge[i + 1][j + 1], -i);

      glNormal3fv((GLfloat*)&(normals_edge[i][j + 1]));
      glTexCoord2fv(texcoords[3]);
      glVertex3f(j + 1.0f, heightmap_edge[i][j + 1], -i + 1.0f);
    }
    glEnd();
  }
}

void
Map::draw_scenery(const float& elapsed) const
{
  for (scenery_t entry : scenery) {
    glPushMatrix();
    glTranslatef(entry.tx, entry.ty, entry.tz);
    glRotatef(entry.angle, entry.rx, entry.ry, entry.rz);
    glScalef(entry.sx, entry.sy, entry.sz);
    entry.model->normalize();
    entry.model->draw(elapsed);
    glPopMatrix();
  }
}

void
Map::draw_square(const int& x, const int& y) const
{
  static const GLfloat texcoords[4][2] = {
    { 0.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f }
  };

  GLfloat emission[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  if (y == highlighted.y && x == highlighted.x) {
    emission[0] = emission[1] = emission[2] = 0.8f;
  }
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);

  int textureid = (path->has_coord(x, y)) ? path->textureid : 0;
  glBindTexture(GL_TEXTURE_2D, textures[textureid]);

  glBegin(GL_TRIANGLE_STRIP);
  glNormal3fv((GLfloat*)&(normals[y][x]));
  glTexCoord2fv(texcoords[0]);
  glVertex3f(x, heightmap[y][x], y);

  glNormal3fv((GLfloat*)&(normals[y + 1][x]));
  glTexCoord2fv(texcoords[1]);
  glVertex3f(x, heightmap[y + 1][x], y + SQSIZE);

  glNormal3fv((GLfloat*)&(normals[y][x + 1]));
  glTexCoord2fv(texcoords[2]);
  glVertex3f(x + SQSIZE, heightmap[y][x + 1], y);

  glNormal3fv((GLfloat*)&(normals[y + 1][x + 1]));
  glTexCoord2fv(texcoords[3]);
  glVertex3f(x + SQSIZE, heightmap[y + 1][x + 1], y + SQSIZE);
  glEnd();
}

void
Map::draw(const float& elapsed) const
{
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_COLOR_MATERIAL);

  GLfloat
    diffuse[]  = { 0.8f, 0.8f, 0.8f, 1.0f },
    specular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
    ambient[]  = { 0.01f, 0.01f, 0.01f, 1.0f };

  glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

  for (size_t h = 0; h < height; ++h) {
    for (size_t w = 0; w < width; ++w) {
      draw_square(w, h);
    }
  }

  const std::array<std::array<float, 4>, 4> rotations { {
    { { 0.0f,   0.0f, 0.0f, 0.0f } },
    { { 90.0f,  0.0f, 1.0f, 0.0f } },
    { { 180.0f, 0.0f, 1.0f, 0.0f } },
    { { 270.0f, 0.0f, 1.0f, 0.0f } }
  } };
  const std::array<std::array<float, 3>, 4> translations { {
    { {  0.0f,          0.0f,  0.0f          } },
    { {  -1.0f * width, 0.0f,  0.0f          } },
    { {  -1.0f * width, 0.0f, -1.0f * height } },
    { {  0.0f,          0.0f, -1.0f * width  } }
  } };

  GLfloat emission[] = { 0.5f, 0.5f, 0.5f, 1.0f };
  glMaterialfv(GL_FRONT, GL_EMISSION, emission);

  for (int i = 0; i < rotations.size(); ++i) {
    glPushMatrix();
    glRotatef(rotations[i][0], rotations[i][1], rotations[i][2], rotations[i][3]);
    glTranslatef(translations[i][0], translations[i][1], translations[i][2]);
    draw_edge_wall();
    glPopMatrix();
  }

  draw_scenery(elapsed);
}


void
Map::draw_normals() const
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);

  for (size_t h = 0; h < height + 1; ++h) {
    for (size_t w = 0; w < width + 1; ++w) {
      glBegin(GL_LINES);
      Vector3 n = normals[h][w];
      n /= 4;
      glVertex3f(w, heightmap[h][w], h);
      glVertex3f(w + n.x, heightmap[h][w] + n.y, h + n.z);
      glEnd();
    }
  }

  glColor3f(0.5f, 0.5f, 0.5f);
  glEnable(GL_DEPTH_TEST);
}

Vector3
Map::get_center_of(int x, int y) const
{
  if (x > width || y > height)
    throw Exception("Can't get center of coords out of map boundaries");

  float dx = (float)x + SQSIZE / 2.0f;
  float dy = (
    heightmap[y][x] + heightmap[y][x + 1] + heightmap[y + 1][x] + heightmap[y + 1][x + 1]
  ) / 4;
  float dz = (float)y + SQSIZE / 2.0f;

  return Vector3(dx, dy, dz);
}

const Path *
Map::get_path() const
{
  return path;
}

size_t
Map::get_width() const
{
  return width;
}

size_t
Map::get_height() const
{
  return height;
}

void
Map::set_highlight(const int& x, const int& y)
{
  highlighted.x = x;
  highlighted.y = y;
}

Vector2
Map::get_highlight() const
{
  return highlighted;
}

Map&
Map::instance()
{
  static Map instance;
  return instance;
}
