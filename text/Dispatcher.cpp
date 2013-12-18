#include "text/Dispatcher.h"
#include "engine/Video.h"
#include "gl/Transform.h"
#include "Game.h"

namespace text {

Dispatcher::Dispatcher()
{
  engine::Engine::instance().on("tick_nodepth", std::bind(&Dispatcher::tick, this));
}

Dispatcher&
Dispatcher::instance()
{
  static Dispatcher instance;
  return instance;
}

void
Dispatcher::clear()
{
  scrollings.clear();
}

void
Dispatcher::overlay(
  const std::string& text,
  const int& x,
  const int& y,
  utils::Color clr,
  const float& ptsize,
  bool offbottom,
  bool offleft
)
{
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glPushAttrib(GL_ENABLE_BIT);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnable(GL_TEXTURE_2D);

  if (ptsize > Text::OVERLAY_PTSIZE) {
    DBGWRN("Requested overlay pt size is large than loaded. "
           "Will upscale which might look horrible. "
           "Consider increasing loaded size.");
  }

  int w, h;
  GLuint texture = Text::create_texture(text, Text::instance().font_overlay, clr, w, h);
  if (0 == texture) {
    DBGWRN("Could not create texture from text: " << text);
    return;
  }

  h *= Text::size_factor(ptsize);
  w *= Text::size_factor(ptsize);

  engine::resolution res = engine::Video::instance().get_resolution();
  float yoffset = (offbottom) ? 2 * y : res.height;
  float xoffset = (offleft)   ? 2 * x : res.width;

  std::vector<GLfloat> data {
  /*  x                y               u     v     r     g     b */
    xoffset - x,     yoffset - y,     0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    xoffset - x,     yoffset - y - h, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    xoffset - x + w, yoffset - y,     1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    xoffset - x + w, yoffset - y - h, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
  };

  glBindTexture(GL_TEXTURE_2D, texture);
  glVertexPointer  (2, GL_FLOAT, 7 * sizeof(GLfloat), (GLfloat*)&data[0]);
  glTexCoordPointer(2, GL_FLOAT, 7 * sizeof(GLfloat), (GLfloat*)&data[2]);
  glColorPointer   (3, GL_FLOAT, 7 * sizeof(GLfloat), (GLfloat*)&data[4]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDeleteTextures(1, &texture);

  glPopClientAttrib();
  glPopAttrib();
}

void
Dispatcher::scrolling(const std::string& text, const glm::vec3& pos, utils::Color clr)
{
  WorldText wt;

  wt.texture = Text::create_texture(
    text,
    Text::instance().font_world,
    clr,
    wt.width,
    wt.height
  );
  if (0 == wt.texture) {
    DBGWRN("Could not create scrolling texture from text: " << text);
    return;
  }

  wt.pos   = pos;
  wt.delta = glm::vec3(0.0f, 0.0f, 0.0f);
  wt.color = clr;

  scrollings.push_back(wt);
}

void
Dispatcher::tick()
{
  if (scrollings.begin() == scrollings.end()) {
    /* No reason to go futher if there is nothing to do */
    return;
  }

  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glPushAttrib(GL_ENABLE_BIT);

  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  auto it = scrollings.begin();
  while (it != scrollings.end()) {
    WorldText& wt = *it;

    glPushMatrix();
    glTranslatef(wt.pos.x + wt.delta.x, wt.pos.y + wt.delta.y, wt.pos.z + wt.delta.z);
    gl::Transform::billboard();
    gl::ShaderProgram::push(gl::ShaderProgram::PROGRAM_TEXTURE);

    float height = 0.25f;
    float width = (float)wt.width/(float)wt.height * height;

    std::vector<GLfloat> data {
    /* x           y         z     u     v */
      0.0f,  0.1f + height, 0.0f, 0.0f, 0.0f,
      0.0f,  0.1f,          0.0f, 0.0f, 1.0f,
      width, 0.1f + height, 0.0f, 1.0f, 0.0f,
      width, 0.1f,          0.0f, 1.0f, 1.0f
    };

    glVertexPointer  (3, GL_FLOAT, 5 * sizeof(GLfloat), (GLfloat*)&data[0]);
    glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(GLfloat), (GLfloat*)&data[3]);
    glBindTexture(GL_TEXTURE_2D, wt.texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    gl::ShaderProgram::pop();
    wt.delta += 0.005;

    it = (glm::length(wt.delta) >= 0.5f) ? scrollings.erase(it) : it + 1;
    glPopMatrix();
  }

  glPopClientAttrib();
  glPopAttrib();
}

}
