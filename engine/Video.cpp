#include "engine/Video.h"
#include "Text.h"
#include "hud/HUD.h"
#include "ui/UI.h"
#include "Debug.h"

B_NS_ENGINE

Video::Video()
{
  res.width  = 1200;
  res.height = 900;
}

resolution
Video::get_resolution() const
{
  return res;
}

void
Video::set_resolution(const int& width, const int& height)
{
  DBGWRN("Trying to change resolution, but it doesn't work. Fix it, or dont do it!");

  res.width = width;
  res.height = height;

  ui::UI::instance().res_change(width, height);
  SDL_SetVideoMode(res.width, res.height, 16, SDL_OPENGL | SDL_SWSURFACE | SDL_NOFRAME);

  Text::init();
  HUD::HUD::init();
}

Video&
Video::instance()
{
  static Video instance;
  return instance;
}

E_NS_ENGINE
