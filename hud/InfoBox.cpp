#include "hud/InfoBox.h"
#include "hud/HUD.h"
#include "text/Dispatcher.h"
#include "Bar.h"
#include "gl/Transform.h"
#include "gl/glm.h"

#include <algorithm>
#include <climits>

namespace hud {

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

  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glPushAttrib(GL_ENABLE_BIT);
  gl::Transform::enable2D();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glDisable(GL_TEXTURE_2D);

  if (followmouse) {
    left = mousex;
    top  = mousey;
    switch (snap) {
      case SNAP_BOTLEFT: top -= height;                 break;
      case SNAP_TOPLEFT:                                break;
      case SNAP_TOPRIGHT: left -= width;                break;
      case SNAP_BOTRIGHT: left -= width; top -= height; break;
      case SNAP_CENTER: /* XXX: Fixme */                break;
    }
  }

  int basey = HUD::screen_height - top;
  int basex = left;

  std::vector<GLfloat> data {
    basex,                           basey,                            0.4f, 0.4f, 0.8f, 0.7f,
    basex,                           basey - height - 2 * BOX_PADDING, 0.4f, 0.4f, 0.8f, 0.7f,
    basex + width + 2 * BOX_PADDING, basey,                            0.4f, 0.4f, 0.8f, 0.7f,
    basex + width + 2 * BOX_PADDING, basey - height - 2 * BOX_PADDING, 0.4f, 0.4f, 0.8f, 0.7f
  };

  glVertexPointer(2, GL_FLOAT, 6 * sizeof(GLfloat), (GLfloat*)&data[0]);
  glColorPointer (4, GL_FLOAT, 6 * sizeof(GLfloat), (GLfloat*)&data[2]);

  glTranslatef(0.0f, 2 * BOX_PADDING, 0.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glTranslatef(basex + BOX_PADDING, basey - height - BOX_PADDING, 0.0f);
  stream.draw();

  gl::Transform::disable2D();
  glPopClientAttrib();
  glPopAttrib();
}

}
