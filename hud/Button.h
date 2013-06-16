#ifndef __HUD__BUTTON_H__
#define __HUD__BUTTON_H__

#include "hud/HUD.h"
#include "Game.h"

#include <de.h>
#include <OpenGL/gl.h>

BEGIN_NS_HUD

class Button;

class Button : public de::Emitter<Button *>
{
public:
  enum LOCATION {
    LOCATION_LEFT,
    LOCATION_RIGHT,
    LOCATION_END
  };

private:
  de::Emitter<GameEvent>::id_t mousedown, mousemotion;
  GLuint texture;
  bool marked;
  int index;
  float left, right, top, bot;
  LOCATION loc;

  bool mouse_in_turf;

  void calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const;
  void signal(std::string signal, const GameEvent& ge);

public:
  Button(GLuint texture);
  ~Button();

  void set_texture(GLuint texture);
  void set_index(int index);
  int get_index() const;
  LOCATION get_location() const;

  void draw() const;
  void mark(bool state = true);
  bool is_marked() const;
  bool point_enclosed(int x, int y) const;
};

END_NS_HUD

#endif
