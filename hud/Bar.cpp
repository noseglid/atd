#include "Bar.h"

#include "Debug.h"

BEGIN_NS_HUD

Bar::Bar() : marked_button(-1)
{
  nbuttonsat[BUTTON_LOCATION_LEFT]  = 0;
  nbuttonsat[BUTTON_LOCATION_RIGHT] = 0;
}

int
Bar::button_index(int x, int y) const
{
  y = HUD::screen_height - y;
  for (button_def def : button_definitions) {
    if (def.vertices.left  <= x &&
        def.vertices.right >= x &&
        def.vertices.bot   <= y &&
        def.vertices.top   >= y) {
      return def.button_index;
    }
  }

  return -1;
}

void
Bar::calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const
{
  int i = abs(xindex) - 1;

  *left = (i + 1) * BAR_PADDING +
          i * (HUD_BUTTON_WIDTH + BAR_PADDING) + BAR_OFFSET;

  *right = (i + 1) * (HUD_BUTTON_WIDTH + 2 * BAR_PADDING) + BAR_OFFSET;

  if (0 > xindex) {
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
  glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(
      BAR_OFFSET,
      yoffset + BAR_HEIGHT
    );

    glVertex2f(
      BAR_OFFSET,
      yoffset
    );

    glVertex2f(
      HUD::screen_width - BAR_OFFSET,
      yoffset + BAR_HEIGHT
    );

    glVertex2f(
      HUD::screen_width - BAR_OFFSET,
      yoffset
    );
  glEnd();
}

void
Bar::draw_button(button_def def) const
{
  glBindTexture(GL_TEXTURE_2D, def.texture);

  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(def.vertices.left, def.vertices.top);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(def.vertices.left, def.vertices.bot);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(def.vertices.right, def.vertices.top);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(def.vertices.right, def.vertices.bot);
  glEnd();

  if (def.button_index == marked_button) {
    /* This button is currently 'marked' */

    glPointSize(12.0f);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(def.vertices.right, def.vertices.top);
    glEnd();
  }
}

bool
Bar::in_turf(int x, int y) const
{
  return
    y > (HUD::screen_height - BAR_OFFSET - BAR_HEIGHT) &&
    y < (HUD::screen_height - BAR_OFFSET);
}

int
Bar::add_button(GLuint texture, button_cb cb, BUTTON_LOCATION loc)
{
  button_def def;
  def.texture = texture;
  def.cb = cb;
  def.button_index = button_definitions.size();
  def.loc = loc;
  calc_button_vertices(
    ((loc == BUTTON_LOCATION_RIGHT) ? -1 : 1) * (++nbuttonsat[loc]),
    &def.vertices.left,
    &def.vertices.right,
    &def.vertices.top,
    &def.vertices.bot
  );

  button_definitions.push_back(def);
  return def.button_index;
}

void
Bar::remove_button(int idx)
{
  auto it = std::find_if(button_definitions.begin(), button_definitions.end(),
    [idx](button_def def) { return def.button_index == idx; });
  if (it == button_definitions.end()) {
    DBGWRN("No button index: " << idx << " defined in HUD.");
    return;
  }

  nbuttonsat[it->loc]--;
  button_definitions.erase(it);
}

void
Bar::mark_button(int bindex)
{
  marked_button = bindex;
}

END_NS_HUD
