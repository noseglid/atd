#include "Map.h"
#include "Debug.h"
#include "Audio.h"
#include "gl/Transform.h"
#include "ImageLoader.h"
#include "Exception.h"
#include "hud/InfoBar.h"

#include <iostream>
#include <iterator>
#include <OpenGL/gl.h>
#include <pjson.hpp>
#include <algorithm>
#include <array>

static GLint texture = 0;

static std::map<std::string, std::array<float, 4>> maptex = {
  { "grass",     { {0.5 / 1024.0,   0.5 / 1024.0, 255.5 / 1024.0, 255.5 / 1024.0} } },
  { "stonepath", { {257.5 / 1024.0, 0.5 / 1024.0, 460.5 / 1024.0, 203.5 / 1024.0} } }
};

Map::Map(const Json::Value& levelspec)
{
  highlighted.x = -1;
  highlighted.y = -1;

  DBG("Registering events for Map");
  engine::Engine& e = engine::Engine::instance();
  events.push_back(e.on("tick",        std::bind(&Map::tick,        this, std::placeholders::_1)));
  events.push_back(e.on("mousemotion", std::bind(&Map::mousemotion, this, std::placeholders::_1)));

  load_level(levelspec);
}

Map::~Map()
{
  delete vbo;
  delete path;

  using engine::Engine;
  for (engine::Engine::id_t ev : events) {
    engine::Engine::instance().off(ev);
  }

  Audio::instance().stop_music();
}

void
Map::load_level(const Json::Value& levelspec)
{
  /* Load metadata */
  width  = levelspec["width"].asInt();
  height = levelspec["height"].asInt();

  /* Load the textures */
  if (0 == texture) {
    texture = IL::GL::texture("map.png");
  }

  /* Load the paths */
  Json::Array paths = levelspec["paths"].asArray();
  if (paths.size() != 1) {
    throw Exception("Only one path per levelspec currently supported.");
  }
  Json::Value p = paths[0];
  path = new Path(p["texture"].asInt(), p["path"].asString());

  /* Prepare the map and its vertices */
  generate_map();

  /* Load the scenery */
  if (levelspec.objectHasKey("scenery")) {
    for (Json::Value entry : levelspec["scenery"].asArray()) {
      scenery_t s;
      std::string modelpath = "resources/models/" + entry["model"].asString() + ".dae";
      s.model = Model::load(modelpath);
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

  Mix_Music *bgmusic = Audio::instance().load_music(levelspec["bgmusic"].asString());
  Audio::instance().play(bgmusic);

  hud::InfoBar::instance().set_title(levelspec["name"].asString());

  /* Set camera limits */
  Camera::instance().set_limits(
    1.0f, width - 1.0f,
    4.0f, 12.0f,
    1.0f, height - 1.0f
  );
}

void
Map::generate_map()
{
  float xstart = -60.0f, xend = width + 60.0f,  xstep = 1.0f;
  float zstart = -60.0f, zend = height + 60.0f, zstep = 1.0f;

  vbo = new gl::VBO();

  std::vector<GLfloat> vertices, normals, texcoords;
  std::vector<GLushort> indices;

  /* Setting all the vertices and texcoords, excluding the path */
  std::array<float, 4> texgrass = maptex["grass"];
  for (float x = xstart; x < xend; x += xstep) {
    for (float z = zstart; z < zend; z += zstep) {
      vertices.push_back(x);
      vertices.push_back((float)rand()/(float)RAND_MAX / 3.0f);
      vertices.push_back(z);

      texcoords.push_back((0 == (size_t)x % 2) ? texgrass[0] : texgrass[2]);
      texcoords.push_back((0 == (size_t)z % 2) ? texgrass[3] : texgrass[1]);
    }
  }

  /* Setting all normals and draw order (indices) */
  float h = (zend - zstart);
  for (size_t x = 0; x < xend - xstart; x++) {
    for (size_t z = 0; z < zend - zstart; z++) {
      glm::vec3 res(0.0, 1.0, 0.0);
      if (0 < x && 0 < z) {
        glm::vec3 v1(0.0,    vertices[3 * (x * h       + z - 1) + 1], zstep);
        glm::vec3 v2(xstep,  vertices[3 * ((x + 1) * h + z) + 1],     0.0);
        glm::vec3 v3(0.0,    vertices[3 * (x * h       + z + 1) + 1], -zstep);
        glm::vec3 v4(-xstep, vertices[3 * ((x - 1) * h + z) + 1],     0.0);
        res = glm::normalize(
          glm::cross(v1, v2) + glm::cross(v2, v3) + glm::cross(v3, v4) + glm::cross(v4, v1)
        );
      }

      normals.push_back(res.x);
      normals.push_back(res.y);
      normals.push_back(res.z);

      /* Don't draw path here. Texturecoords require vertex duplication */
      if (path->has_coord(x + xstart, z + zstart)) {
        continue;
      }

      /* Can not set indice for last row/column. */
      if (xend - xstart - 1 <= x || zend - zstart - 1 <= z) {
        continue;
      }

      indices.push_back(x * h       + z);
      indices.push_back(x * h       + z + 1);
      indices.push_back((x + 1) * h + z + 1);

      indices.push_back((x + 1) * h + z + 1);
      indices.push_back((x + 1) * h + z);
      indices.push_back(x * h       + z);
    }
  }

  /* Setting the path */
  PathCoord c = path->get_start();
  std::array<float, 4> texstonepath = maptex["stonepath"];
  int mapoffset = ((xend - xstart) * (zend - zstart));
  int index = 0;
  while (true) {
    int msx = c.x - xstart;
    int msz = c.y - zstart;
    vertices.push_back(c.x);
    vertices.push_back(vertices[3 * (msx * h + msz) + 1]);
    vertices.push_back(c.y);
    normals.push_back(normals[3 * (msx * h + msz) + 0]);
    normals.push_back(normals[3 * (msx * h + msz) + 1]);
    normals.push_back(normals[3 * (msx * h + msz) + 2]);
    texcoords.push_back(texstonepath[0]);
    texcoords.push_back(texstonepath[1]);

    vertices.push_back(c.x);
    vertices.push_back(vertices[3 * (msx * h + (msz + zstep)) + 1]);
    vertices.push_back(c.y + zstep);
    normals.push_back(normals[3 * (msx * h + (msz + zstep)) + 0]);
    normals.push_back(normals[3 * (msx * h + (msz + zstep)) + 1]);
    normals.push_back(normals[3 * (msx * h + (msz + zstep)) + 2]);
    texcoords.push_back(texstonepath[0]);
    texcoords.push_back(texstonepath[3]);

    vertices.push_back(c.x + xstep);
    vertices.push_back(vertices[3 * ((msx + xstep) * h + (msz + zstep)) + 1]);
    vertices.push_back(c.y + zstep);
    normals.push_back(normals[3 * ((msx + xstep) * h + (msz + zstep)) + 0]);
    normals.push_back(normals[3 * ((msx + xstep) * h + (msz + zstep)) + 1]);
    normals.push_back(normals[3 * ((msx + xstep) * h + (msz + zstep)) + 2]);
    texcoords.push_back(texstonepath[2]);
    texcoords.push_back(texstonepath[3]);

    vertices.push_back(c.x + xstep);
    vertices.push_back(vertices[3 * ((msx + xstep) * h + (msz)) + 1]);
    vertices.push_back(c.y);
    normals.push_back(normals[3 * ((msx + xstep) * h + msz) + 0]);
    normals.push_back(normals[3 * ((msx + xstep) * h + msz) + 1]);
    normals.push_back(normals[3 * ((msx + xstep) * h + msz) + 2]);
    texcoords.push_back(texstonepath[2]);
    texcoords.push_back(texstonepath[1]);

    indices.push_back(mapoffset + index + 0);
    indices.push_back(mapoffset + index + 1);
    indices.push_back(mapoffset + index + 2);

    indices.push_back(mapoffset + index + 0);
    indices.push_back(mapoffset + index + 2);
    indices.push_back(mapoffset + index + 3);

    index += 4;

    try {
      c = path->next_coord(c);
    } catch (const NoMoreCoords& e) {
      /* Done and done */
      break;
    }
  }

  gl::Material m(glm::vec4(1.0, 1.0, 0.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0));

  DBG("Map created, vertices: " << (vertices.size() / 3)
    << ", triangles: " << (indices.size() / 3));

  vbo->bind_indices(indices);
  vbo->bind_data(gl::VBO::VERTEX, vertices);
  vbo->bind_data(gl::VBO::TEXCOORD, texcoords);
  vbo->bind_data(gl::VBO::NORMAL, normals);
  vbo->set_texture(texture);
}

void
Map::tick(const engine::Event& ge)
{
  this->draw(ge.elapsed);
}

void
Map::mousemotion(const engine::Event& ge)
{
  static MapEvent me;

  glm::vec3 v = gl::Transform::unproject(ge.ev.motion.x, ge.ev.motion.y);
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
Map::draw(const float& elapsed) const
{
  vbo->draw();

  for (scenery_t entry : scenery) {
    glPushMatrix();
      glm::mat4 m(1.0);
      m = glm::translate(m, glm::vec3(entry.tx, entry.ty, entry.tz));
      m = glm::rotate(m, entry.angle, glm::vec3(entry.rx, entry.ry, entry.rz));
      m = glm::scale(m, glm::vec3(entry.sx, entry.sy, entry.sz));
      glMultMatrixf(&m[0][0]);
      entry.model->normalize();
      entry.model->draw(elapsed);
    glPopMatrix();
  }
}

glm::vec3
Map::get_center_of(int x, int y) const
{
  if (x > width || y > height)
    throw Exception("Can't get center of coords out of map boundaries");

  return glm::vec3((float)x + 0.5f, 0.0, (float)y + 0.5f);
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

glm::vec2
Map::get_highlight() const
{
  return highlighted;
}
