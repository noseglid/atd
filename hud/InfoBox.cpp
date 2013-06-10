#include "hud/InfoBox.h"
#include "GLTransform.h"
#include "Text.h"
#include "Bar.h"

#include <algorithm>

BEGIN_NS_HUD

#define TEXT_MARGIN 3.0f
#define TEXT_PADDING 12.0f

#define PTSIZE 24.0f

InfoBox::InfoBox()
{
  Game::instance().on("tick_nodepth", std::bind(&InfoBox::tick, this));
  Game::instance().on("mousemotion", std::bind(&InfoBox::mousemotion, this, std::placeholders::_1));
}

InfoBox&
InfoBox::instance()
{
  static InfoBox instance;
  return instance;
}

InfoBox::id_t
InfoBox::add_box(std::string text, bool follow_mouse, int snap)
{
  boxdef entry;
  entry.width = entry.height = entry.line_height = 0;
  entry.snap = snap;
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

  entry.x = follow_mouse ? -1 : HUD::screen_width - (entry.width + BAR_OFFSET);
  entry.y = follow_mouse ? -1 : HUD::screen_height - (entry.height + BAR_HEIGHT + 2 * BAR_OFFSET);

  return boxlist.insert(boxlist.begin(), entry);
}

void
InfoBox::remove_box(id_t& id)
{
  if (id == boxlist.end()) {
    return;
  }

  boxlist.erase(id);
  id = boxlist.end();
}

InfoBox::id_t
InfoBox::noid()
{
  return boxlist.end();
}

void
InfoBox::tick() const
{
  GLTransform::enable2D();

  for (boxdef def : boxlist) {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor4f(0.4f, 0.4f, 0.8f, 0.7f);

    int x = def.x;
    int y = def.y;
    if (-1 == def.x && -1 == def.y) {
      x = mousex;
      y = mousey;
      switch (def.snap) {
        case 0:
          y -= def.height;
          break;
        case 1:
          break;
        case 2:
          x -= def.width;
          break;
        case 3:
          x -= def.width;
          y -= def.height;
          break;
        default:
          DBGWRN("Invalid snap value, defaulting to 0: " << def.snap);
          break;
      }
    }

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(x, HUD::screen_height - (y));
    glVertex2f(x, HUD::screen_height - (y + def.height));
    glVertex2f(x + def.width, HUD::screen_height - (y));
    glVertex2f(x + def.width, HUD::screen_height - (y + def.height));
    glEnd();

    int pos = HUD::screen_height - (y + TEXT_MARGIN);
    for (std::string line : def.lines) {
      pos -= def.line_height;
      Text::overlay(line, x + TEXT_PADDING, pos, PTSIZE);
    }
  }

  GLTransform::disable2D();
}

void
InfoBox::mousemotion(const GameEvent& ge)
{
  mousex = ge.ev.motion.x;
  mousey = ge.ev.motion.y;
}

END_NS_HUD
