#include "InfoBar.h"
#include "engine/Engine.h"
#include "Text.h"
#include "Player.h"
#include "GLTransform.h"

#include <sstream>
#include <algorithm>

B_NS_HUD

#define TITLE_PTSIZE 48.0f
#define INFO_PTSIZE 20.0f

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
InfoBar::set_info_text(std::vector<std::string>& vector, std::string text)
{
  vector.clear();

  std::stringstream ss(text);
  std::string elem;
  while (std::getline(ss, elem)) {
    vector.push_back(elem);
  }
}

void
InfoBar::set_info_text1(std::string text)
{
  set_info_text(info_text1, text);
}

void
InfoBar::set_info_text2(std::string text)
{
  set_info_text(info_text2, text);
}

void
InfoBar::draw_info_text(bool leftjustify, const std::vector<std::string>& text) const
{
  if (text.empty()) return;

  int maxwidth = 0, maxheight = 0;
  for (std::string line : text) {
    int width, height;
    Text::size(line, &width, &height, INFO_PTSIZE);
    maxwidth  = std::max(maxwidth, width);
    maxheight = std::max(maxheight, height);
  }

  Text::set_color(utils::colors::white);

  int left = leftjustify ? 10.0f : HUD::screen_width - 10.0f - maxwidth;

  int i = 0;
  for (std::string line : text) {
    Text::overlay(line, left, (i++) * maxheight + 5.0f, INFO_PTSIZE, false);
  }
}

void
InfoBar::tick() const
{
  if (!visible) return;

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

  draw_info_text(true, info_text1);
  draw_info_text(false, info_text2);

  GLTransform::disable2D();
}

E_NS_HUD
