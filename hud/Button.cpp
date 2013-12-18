#include "hud/Button.h"
#include "hud/Bar.h"
#include "Debug.h"

#include <OpenGL/gl.h>

namespace hud {

Button::Button(GLuint texture) : texture(texture), mouse_in_turf(false)
{
  engine::Engine& e = engine::Engine::instance();

  mousedown   = e.on("mousedown", std::bind(&Button::signal, this, "click", std::placeholders::_1));
  mousemotion = e.on("mousemotion", std::bind(&Button::signal, this, "hover", std::placeholders::_1));
}

Button::~Button()
{
  engine::Engine& e = engine::Engine::instance();
  e.off(mousedown);
  e.off(mousemotion);
}

void
Button::signal(std::string signal, const engine::Event& ge)
{
  int x = ge.ev.button.x;
  int y = HUD::screen_height - ge.ev.button.y;

  bool is_inside = point_enclosed(x, y);
  bool was_inside = mouse_in_turf;
  mouse_in_turf = is_inside;

  if (was_inside && !is_inside) {
    emit("mouseexit", this);
  }

  if (!was_inside && is_inside) {
    emit("mouseentry", this);
  }

  if (is_inside) {
    std::string osignal = "click";
    switch (ge.ev.button.button) {
      case SDL_BUTTON_LEFT:   osignal = "leftclick";   break;
      case SDL_BUTTON_MIDDLE: osignal = "middleclick"; break;
      case SDL_BUTTON_RIGHT:  osignal = "rightclick";  break;
    }
    emit(osignal, this);
  }
}

void
Button::set_texture(GLuint texture)
{
  this->texture = texture;
}

void
Button::set_index(int index)
{
  this->index = index;
  loc = (0 > index) ? LOCATION_RIGHT : LOCATION_LEFT;
  Bar::calc_button_vertices(index, &left, &right, &top, &bot);
}

int
Button::get_index() const
{
  return index;
}

Button::LOCATION
Button::get_location() const
{
  return loc;
}

bool
Button::point_enclosed(int x, int y) const
{
  return (left <= x && right >= x && bot <= y && top >= y);
}

void
Button::draw() const
{
  glPushAttrib(GL_ENABLE_BIT);
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glBindTexture(GL_TEXTURE_2D, texture);

  std::vector<GLfloat> vertices {
    left, top,
    left, bot,
    right, top,
    right, bot
  };

  const static std::vector<GLfloat> texcoords {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
  };

  glVertexPointer(2, GL_FLOAT, 0, (GLfloat*)&vertices[0]);
  glTexCoordPointer(2, GL_FLOAT, 0, (GLfloat*)&texcoords[0]);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glPopClientAttrib();
  glPopAttrib();
}

}
