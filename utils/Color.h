#ifndef __UTILS_COLOR_H__
#define __UTILS_COLOR_H__

#include <SDL/SDL.h>

namespace utils {

class Color
{
public:
  float r, g, b;

  Color();
  Color(float r, float g, float b);

  SDL_Color to_sdl() const;

  static Color interpolate_progress(float factor);
};

namespace colors {
  extern Color red;
  extern Color green;
  extern Color blue;
  extern Color yellow;
  extern Color white;
  extern Color black;
  extern Color gray;
  extern Color lightgray;
  extern Color orange;
  extern Color purple;
  extern Color gold;
}

}

#endif
