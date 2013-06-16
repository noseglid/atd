#include "hud/InfoBox.h"
#include "GLTransform.h"
#include "Bar.h"

#include <algorithm>
#include <climits>

BEGIN_NS_HUD

InfoBox::color InfoBox::red     (1.00f, 0.00f, 0.00f);
InfoBox::color InfoBox::green   (0.00f, 1.00f, 0.00f);
InfoBox::color InfoBox::blue    (0.00f, 0.00f, 1.00f);
InfoBox::color InfoBox::white   (1.00f, 1.00f, 1.00f);
InfoBox::color InfoBox::black   (0.00f, 0.00f, 0.00f);
InfoBox::color InfoBox::orange  (1.00f, 0.50f, 0.00f);

static int mousex, mousey;

void
static mousemotion(const GameEvent& ge)
{
  /* Connot set member variables here as it might be cancelled in TowerManager::button_mouse_event */
  mousex = (int)ge.ev.motion.x;
  mousey = (int)ge.ev.motion.y;
}

InfoBox::InfoBox(SNAP snap, bool followmouse) :
  left(INT_MAX), top(INT_MAX), snap(snap), followmouse(followmouse),
  inputx(BOX_PADDING), inputy(BOX_PADDING), input_ptsize(18.0f),
  input_indent(0.0f), inputr(1.0f), inputg(1.0f), inputb(1.0f)
{
  auto fn = std::bind(mousemotion, std::placeholders::_1);
  mousemotion_emit = Game::instance().on("mousemotion", fn);
}

InfoBox::~InfoBox()
{
  Game::instance().off(mousemotion_emit);
}

void
InfoBox::set_bounding_box()
{
  /* FIXME: Set static location accordingly if not following mouse. */

  height = BOX_PADDING;
  width  = BOX_PADDING;

  for (boxcontent c : content) {
    int w, h;
    Text::size(c.text, &w, &h, c.ptsize);
    height = std::max(height, c.y + h);
    width  = std::max(width,  c.x + w);
  }

  int lr = HUD::screen_width - width - BOX_MARGIN - 2 * BOX_PADDING;
  int ll = BOX_MARGIN;
  int tt = BAR_OFFSET + BAR_HEIGHT + BOX_MARGIN + 2 * BOX_PADDING;
  int tb = HUD::screen_height - height - BAR_OFFSET - BAR_HEIGHT - BOX_MARGIN;
  if (!followmouse) {
    switch (snap) {
      case SNAP_BOTLEFT:
        left = ll;
        top = tb;
        break;
      case SNAP_TOPLEFT:
        left = ll;
        top = tt;
        break;
      case SNAP_TOPRIGHT:
        left = lr;
        top = tt;
        break;
      case SNAP_BOTRIGHT:
        left = lr;
        top = tb;
        break;
    }
  }
}

void
InfoBox::clear()
{
  content.clear();
  inputx = inputy = BOX_PADDING;
  top = left = INT_MAX;
}

#if 0
InfoBox::id_t
InfoBox::add_box(std::string text, bool follow_mouse, int snap)
{
  boxdef entry;
  entry.width = 80;
  entry.height = 140;
  entry.snap = snap;

  boxcontent content;
  content.color = Vector3(1.0f, 0.5f, 0.0f);
  content.ptsize = 24.0f;
  content.x = 0.0f;
  content.y = 0.0f;
  content.text = "Arcane arcate";

  entry.content.push_back(content);

  /*
  std::stringstream ss(text);
  std::string elem;
  while (std::getline(ss, elem)) {
    entry.lines.push_back(elem);

    int width = 0, height = 0;
    Text::size(elem, &width, &height, PTSIZE);

    entry.width       = std::max(width, entry.width);
    entry.height     += height;
    entry.line_height = std::max(height, entry.line_height);
  }

  entry.width  += 2 * TEXT_PADDING;
  entry.height += 2 * TEXT_MARGIN;

  */
  entry.x = follow_mouse ? -1 : HUD::screen_width - (entry.width + BAR_OFFSET);
  entry.y = follow_mouse ? -1 : HUD::screen_height - (entry.height + BAR_HEIGHT + 2 * BAR_OFFSET);

  return boxlist.insert(boxlist.begin(), entry);
}
#endif

void
InfoBox::draw()
{
  GLTransform::enable2D();

  if (followmouse) {
    left = mousex;
    top  = mousey;
    switch (snap) {
      case SNAP_BOTLEFT: top -= height;                 break;
      case SNAP_TOPLEFT:                                break;
      case SNAP_TOPRIGHT: left -= width;                break;
      case SNAP_BOTRIGHT: left -= width; top -= height; break;
    }
  }

  int basey = HUD::screen_height - top;
  int basex = left;

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glColor4f(0.4f, 0.4f, 0.8f, 0.7f);
  glTranslatef(0.0f, 2 * BOX_PADDING, 0.0f);

  glBegin(GL_TRIANGLE_STRIP);
  glVertex2f(basex,         basey);
  glVertex2f(basex,         basey - height - 2 * BOX_PADDING);
  glVertex2f(basex + width + 2 * BOX_PADDING, basey);
  glVertex2f(basex + width + 2 * BOX_PADDING, basey - height - 2 * BOX_PADDING);
  glEnd();

  for (boxcontent c : content) {
    Text::set_color(c.r, c.g, c.b);
    Text::overlay(c.text, basex + c.x + BOX_PADDING, basey - c.y + BOX_PADDING, c.ptsize);
  }

  GLTransform::disable2D();
}

END_NS_HUD
