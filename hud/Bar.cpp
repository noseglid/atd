#include "hud/HUD.h"
#include "hud/Bar.h"
#include "Debug.h"

#include <stdlib.h>
#include <OpenGL/gl.h>

namespace hud {

Bar::Bar() : visible(false)
{
}

void
Bar::calc_button_vertices(int index, float *left, float *right, float *top, float *bot)
{
  int i = abs(index) - 1;

  *left = (i + 1) * BAR_PADDING +
          i * (HUD_BUTTON_WIDTH + BAR_PADDING) + BAR_OFFSET;

  *right = (i + 1) * (HUD_BUTTON_WIDTH + 2 * BAR_PADDING) + BAR_OFFSET;

  if (0 > index) {
    int tmp = *left;
    *left = HUD::screen_width - *right;
    *right = HUD::screen_width - tmp;
  }

  *top = BAR_OFFSET + HUD_BUTTON_HEIGHT + BAR_PADDING;
  *bot = BAR_OFFSET + BAR_PADDING;
}

void
Bar::draw_banner(float yoffset) const
{
  if (!visible) return;

  glBegin(GL_TRIANGLE_STRIP);
  glVertex2f(BAR_OFFSET,                     yoffset + BAR_HEIGHT);
  glVertex2f(BAR_OFFSET,                     yoffset             );
  glVertex2f(HUD::screen_width - BAR_OFFSET, yoffset + BAR_HEIGHT);
  glVertex2f(HUD::screen_width - BAR_OFFSET, yoffset             );
  glEnd();
}

bool
Bar::in_turf(int x, int y) const
{
  return
    y > (HUD::screen_height - BAR_OFFSET - BAR_HEIGHT) &&
    y < (HUD::screen_height - BAR_OFFSET);
}

void
Bar::show()
{
  visible = true;
}

void
Bar::hide()
{
  visible = false;
}

}
