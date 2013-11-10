#ifndef __ENGINE__VIDEO_H__
#define __ENGINE__VIDEO_H__

#include "engine/Engine.h"

B_NS_ENGINE

struct resolution
{
  int width, height;
};

class Video
{
  resolution res;
  SDL_Surface *surface;

  Video();
  ~Video();

public:
  static Video& instance();

  resolution get_resolution() const;
  void set_resolution(const int& width, const int& height);
};

E_NS_ENGINE

#endif
