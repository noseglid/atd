#include "engine/Video.h"
#include "Exception.h"
#include "text/Text.h"
#include "hud/HUD.h"
#include "ui/UI.h"
#include "Debug.h"

B_NS_ENGINE

Video::Video() : surface(NULL)
{
}

Video::~Video()
{
}

resolution
Video::get_resolution() const
{
  return res;
}

void
Video::set_resolution(const int& width, const int& height)
{
  static bool first = true;
  if (!first) {
    DBGWRN("Trying to change resolution, but it doesn't work. Fix it, or dont do it!");
  }

  first = false;

  res.width = width;
  res.height = height;

  surface = SDL_SetVideoMode(res.width, res.height, 16, SDL_OPENGL | SDL_SWSURFACE | SDL_NOFRAME);
  if (NULL == surface) {
    DBGERR("SDL error: " << SDL_GetError());
    throw Exception("Could not set resolution.");
  }

  HUD::HUD::init();
}

Video&
Video::instance()
{
  static Video instance;
  return instance;
}

E_NS_ENGINE
