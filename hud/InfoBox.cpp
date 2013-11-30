#include "hud/InfoBox.h"
#include "text/Dispatcher.h"
#include "GLTransform.h"
#include "Bar.h"
#include "gl/glm.h"

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

InfoBox::InfoBox(text::Stream stream, SNAP snap, bool followmouse) :
  top(0), left(0), width(2 * BOX_PADDING), height(2 * BOX_PADDING),
  stream(stream), snap(snap), followmouse(followmouse)
{
  mousemotion_emit = engine::Engine::instance().on(
    "mousemotion", std::bind(mousemotion, std::placeholders::_1)
  );

  set_bounding_box();
}

InfoBox::~InfoBox()
{
  engine::Engine::instance().off(mousemotion_emit);
}

void
InfoBox::set_content(text::Stream stream)
{
  this->stream = stream;
  set_bounding_box();
}

void
InfoBox::set_bounding_box()
{
  glm::ivec4 streambb = stream.get_bounding_box();
  height = streambb.w;
  width  = streambb.z;

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
  stream.clear();
}

void
InfoBox::draw()
{
  if (stream.empty()) return;

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
    glVertex2f(basex,                           basey);
    glVertex2f(basex,                           basey - height - 2 * BOX_PADDING);
    glVertex2f(basex + width + 2 * BOX_PADDING, basey);
    glVertex2f(basex + width + 2 * BOX_PADDING, basey - height - 2 * BOX_PADDING);
  glEnd();

  glTranslatef(basex + BOX_PADDING, basey - height - BOX_PADDING, 0.0f);
  stream.draw();

  GLTransform::disable2D();
}

E_NS_HUD
