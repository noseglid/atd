#include "InfoBar.h"
#include "Game.h"
#include "Text.h"
#include "Player.h"
#include "GLTransform.h"

BEGIN_NS_HUD

#define TITLE_PTSIZE 48.0f
#define INFO_PTSIZE 24.0f

InfoBar::InfoBar()
{
  Game::instance().on("tick_nodepth", std::bind(&InfoBar::tick, this));
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
InfoBar::set_info_text(std::string text)
{
  info_text.clear();

  std::stringstream ss(text);
  std::string elem;
  while (std::getline(ss, elem)) {
    info_text.push_back(elem);
  }
}

void
InfoBar::tick() const
{
  GLTransform::enable2D();

  glColor4f(BAR_COLOR);
  glDisable(GL_TEXTURE_2D);
  draw_banner(HUD::screen_height - (BAR_HEIGHT + BAR_OFFSET));

  if (!title.empty()) {
    int title_width, title_height;
    Text::set_color(utils::colors::lightgray);
    Text::size(title, &title_width, &title_height, TITLE_PTSIZE);
    Text::overlay(
      title,
      HUD::screen_width / 2 - title_width / 2,
      BAR_OFFSET + BAR_HEIGHT / 2 - title_height / 2,
      TITLE_PTSIZE,
      false
    );
  }

  if (!info_text.empty()) {
    Text::set_color(utils::colors::white);
    int i = 0;
    int line_height;
    Text::size(info_text.at(0), NULL, &line_height, INFO_PTSIZE);
    for (std::string line : info_text) {
      Text::overlay(line, 10.0f, (i++) * line_height + 5.0f, INFO_PTSIZE, false);
    }
  }

  GLTransform::disable2D();
}

END_NS_HUD
