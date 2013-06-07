#include "HUD.h"
#include "InfoBar.h"
#include "ButtonBar.h"

BEGIN_NS_HUD

int HUD::screen_width  = 0;
int HUD::screen_height = 0;

void
HUD::init(int screen_width, int screen_height)
{
  HUD::screen_width  = screen_width;
  HUD::screen_height = screen_height;

  InfoBar::instance();
  ButtonBar::instance();
}

END_NS_HUD
