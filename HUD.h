#ifndef __HUD_H__
#define __HUD_H__

#include "Game.h"

#include <OpenGL/gl.h>
#include <list>

class HUD
{
  typedef std::function<void(int)> button_cb;
  typedef struct {
    button_cb cb;
    GLuint texture;
    int button_index;
  } button_def;
  static int screen_width, screen_height;

  int marked_button;
  std::vector<button_def> button_definitions;

  int button_index(int x, int y) const;

  void tick() const;
  void mousedown(const GameEvent& ev) const;
  void draw_stats() const;
  void draw_bottom_banner() const;
  void draw_button(GLuint texture, int i, bool marked) const;

  HUD();

public:
  static void init(int screen_width, int screen_height);

  bool in_turf(int x, int y) const;
  int add_button(GLuint texture, button_cb cb);
  void mark_button(int index);

  static HUD& instance();
};

#endif
