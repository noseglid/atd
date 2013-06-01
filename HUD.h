#ifndef __HUD_H__
#define __HUD_H__

#include "Game.h"

#include <OpenGL/gl.h>
#include <list>

class HUD
{
public:
  typedef std::function<void(int)> button_cb;

  enum BUTTON_LOCATION {
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    LOCATION_END
  };

private:
  typedef struct {
    button_cb cb;
    GLuint texture;
    int button_index;
    BUTTON_LOCATION loc;

    struct {
      float left, right, top, bot;
    } vertices;

  } button_def;

  static int screen_width, screen_height;

  int nbuttonsat[LOCATION_END];

  int marked_button;
  std::vector<button_def> button_definitions;

  std::string title;

  int button_index(int x, int y) const;

  void tick() const;
  void mousedown(const GameEvent& ev) const;
  void draw_stats() const;
  void draw_banner(float yoffset) const;
  void draw_button(button_def) const;
  void calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const;

  HUD();

public:

  static void init(int screen_width, int screen_height);

  bool in_turf(int x, int y) const;
  int add_button(GLuint texture, button_cb cb, BUTTON_LOCATION loc = BOTTOM_LEFT);
  void remove_button(int index);
  void mark_button(int bindex);

  void set_title(std::string title);

  static HUD& instance();
};

#endif
