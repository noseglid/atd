#include "engine/Video.h"
#include "Exception.h"
#include "text/Text.h"
#include "hud/HUD.h"
#include "ui/UI.h"
#include "Debug.h"

namespace engine {

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

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  surface = SDL_SetVideoMode(res.width, res.height, 32, SDL_OPENGL | SDL_SWSURFACE | SDL_NOFRAME);
  if (NULL == surface) {
    DBGERR("SDL error: " << SDL_GetError());
    throw Exception("Could not set resolution.");
  }

  hud::HUD::init();
}

Video&
Video::instance()
{
  static Video instance;
  return instance;
}

}
