#include "hud/InfoBox.h"
#include "text/Dispatcher.h"
#include "GLTransform.h"
#include "Bar.h"

#include <algorithm>
#include <climits>

B_NS_HUD

static int mousex, mousey;

void
static mousemotion(const engine::Event& ge)
{
  /* Connot set member variables here as it might be cancelled in TowerManager::button_mouse_event */
  mousex = (int)ge.ev.motion.x;
  mousey = (int)ge.ev.motion.y;
}

InfoBox::InfoBox(SNAP snap, bool followmouse) :
  left(INT_MAX), top(INT_MAX), snap(snap), followmouse(followmouse),
  inputx(BOX_PADDING), inputy(BOX_PADDING), input_ptsize(18.0f),
  input_indent(0.0f), clr(1.0f, 1.0f, 1.0f)
{
  mousemotion_emit = engine::Engine::instance().on(
    "mousemotion", std::bind(mousemotion, std::placeholders::_1)
  );
}

InfoBox::~InfoBox()
{
  engine::Engine::instance().off(mousemotion_emit);
}

void
InfoBox::set_bounding_box()
{
  height = BOX_PADDING;
  width  = BOX_PADDING;

  for (boxcontent c : content) {
    int w, h;
    text::Text::size(c.text, &w, &h, c.ptsize);
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
      case SNAP_CENTER:
        left = HUD::screen_width / 2 - width / 2;
        top  = HUD::screen_height / 2 - height / 2;
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

void
InfoBox::draw()
{
  if (0 == content.size()) return;

  GLTransform::enable2D();

  if (followmouse) {
    left = mousex;
    top  = mousey;
    switch (snap) {
      case SNAP_BOTLEFT: top -= height;                 break;
      case SNAP_TOPLEFT:                                break;
      case SNAP_TOPRIGHT: left -= width;                break;
      case SNAP_BOTRIGHT: left -= width; top -= height; break;
      case SNAP_CENTER: /* Que? Do nothing here. */     break;
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
    text::Dispatcher::overlay(
      c.text,
      basex + c.x + BOX_PADDING,
      basey - c.y + BOX_PADDING,
      c.clr,
      c.ptsize);
  }

  GLTransform::disable2D();
}

E_NS_HUD
