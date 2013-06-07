#ifndef __HUD__BAR_H_
#define __HUD__BAR_H_

#include "HUD.h"

#include <OpenGL/gl.h>
#include <functional>
#include <string>
#include <vector>
#include <stdlib.h>

BEGIN_NS_HUD

#define HUD_BUTTON_WIDTH 80.0f
#define HUD_BUTTON_HEIGHT 64.0f

#define BAR_OFFSET 3.0f
#define BAR_PADDING 5.0f
#define BAR_HEIGHT (HUD_BUTTON_HEIGHT + 2 * BAR_PADDING)

#define BAR_COLOR 0.6f, 0.6f, 0.6f, 0.85f

class Bar
{
public:
  enum BUTTON_LOCATION {
    BUTTON_LOCATION_LEFT,
    BUTTON_LOCATION_RIGHT,
    BUTTON_LOCATION_END
  };

private:
  int nbuttonsat[BUTTON_LOCATION_END];
  int marked_button;

  void calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const;

protected:
  typedef std::function<void(int)> button_cb;
  typedef struct {
    button_cb cb;
    GLuint texture;
    int button_index;
    BUTTON_LOCATION loc;

    struct {
      float left, right, top, bot;
    } vertices;

  } button_def;

  std::vector<button_def> button_definitions;

  Bar();
  int button_index(int x, int y) const;
  void draw_banner(float yoffset) const;
  void draw_button(button_def) const;

public:
  bool in_turf(int x, int y) const;
  int add_button(GLuint texture, button_cb cb, BUTTON_LOCATION loc = BUTTON_LOCATION_LEFT);
  void remove_button(int index);
  void mark_button(int bindex);
};

END_NS_HUD

#endif
