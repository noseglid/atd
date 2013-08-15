#ifndef __HUD__BAR_H_
#define __HUD__BAR_H_

#include "HUD.h"

B_NS_HUD

#define HUD_BUTTON_WIDTH 80.0f
#define HUD_BUTTON_HEIGHT 64.0f

#define BAR_OFFSET 3.0f
#define BAR_PADDING 5.0f
#define BAR_HEIGHT (HUD_BUTTON_HEIGHT + 2 * BAR_PADDING)

#define BAR_COLOR 0.6f, 0.6f, 0.6f, 0.85f

class Bar
{
protected:
  bool visible;

  Bar();
  void draw_banner(float yoffset) const;

public:
  bool in_turf(int x, int y) const;
  void show();
  void hide();

  static void calc_button_vertices(int index, float *left, float *right, float *top, float *bot);
};

E_NS_HUD

#endif
