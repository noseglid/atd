#include "HUD.h"
#include "InfoBar.h"
#include "ButtonBar.h"
#include "engine/Video.h"

namespace hud {

int HUD::screen_width  = 0;
int HUD::screen_height = 0;

void
HUD::init()
{
  engine::resolution res = engine::Video::instance().get_resolution();
  HUD::screen_width  = res.width;
  HUD::screen_height = res.height;

  InfoBar::instance();
  ButtonBar::instance();
}

void
HUD::show()
{
  InfoBar::instance().show();
  ButtonBar::instance().show();
}

void
HUD::hide()
{
  InfoBar::instance().hide();
  ButtonBar::instance().hide();
}

}
