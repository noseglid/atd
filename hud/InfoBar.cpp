#include "hud/InfoBar.h"
#include "hud/HUD.h"
#include "engine/Engine.h"
#include "text/Text.h"
#include "text/Dispatcher.h"
#include "Player.h"
#include "gl/Transform.h"

#include <sstream>
#include <algorithm>

namespace hud {

#define TITLE_PTSIZE 48.0f

InfoBar::InfoBar()
{
  engine::Engine::instance().on("tick_nodepth", std::bind(&InfoBar::tick, this));
}

InfoBar&
InfoBar::instance()
{
  static InfoBar instance;
  return instance;
}

void
InfoBar::set_title(std::string title)
{
  this->title = title;
}

void
InfoBar::set_info_text1(text::Stream stream)
{
  info_text2 = stream;
}

void
InfoBar::set_info_text2(text::Stream stream)
{
  info_text1 = stream;
}

void
InfoBar::tick() const
{
  if (!visible) return;

  gl::Transform::enable2D();

  glColor4f(BAR_COLOR);
  glDisable(GL_TEXTURE_2D);
  draw_banner(HUD::screen_height - (BAR_HEIGHT + BAR_OFFSET));

  if (!title.empty()) {
    int title_width, title_height;
    text::Text::size(title, &title_width, &title_height, TITLE_PTSIZE);
    text::Dispatcher::overlay(
      title,
      HUD::screen_width / 2 - title_width / 2,
      BAR_OFFSET + BAR_HEIGHT / 2 - title_height / 2,
      utils::colors::white,
      TITLE_PTSIZE,
      false
    );
  }

  glPushMatrix();
  glLoadIdentity();
  glm::ivec4 bb1 = info_text1.get_bounding_box();
  glTranslatef(HUD::screen_width - (BAR_OFFSET + BAR_PADDING + bb1.z),
               HUD::screen_height - (BAR_OFFSET + BAR_PADDING + bb1.w), 0.0f);
  info_text1.draw();
  glPopMatrix();

  glPushMatrix();
  glm::ivec4 bb2 = info_text2.get_bounding_box();
  glTranslatef(BAR_OFFSET + BAR_PADDING,
               HUD::screen_height - (BAR_OFFSET + BAR_PADDING + bb2.w), 0.0f);
  info_text2.draw();
  glPopMatrix();

  gl::Transform::disable2D();
}

}
