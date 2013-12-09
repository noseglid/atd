#include "Color.h"

namespace utils {

namespace colors {
  Color red       = Color(1.00f, 0.00f, 0.00f);
  Color green     = Color(0.00f, 1.00f, 0.00f);
  Color blue      = Color(0.00f, 0.00f, 1.00f);
  Color yellow    = Color(1.00f, 1.00f, 0.00f);
  Color white     = Color(1.00f, 1.00f, 1.00f);
  Color black     = Color(0.00f, 0.00f, 0.00f);
  Color gray      = Color(0.50f, 0.50f, 0.50f);
  Color lightgray = Color(0.80f, 0.80f, 0.80f);
  Color orange    = Color(1.00f, 0.50f, 0.00f);
  Color purple    = Color(1.00f, 0.50f, 1.00f);
  Color gold      = Color(1.00f, 0.90f, 0.00f);
}

Color::Color() : Color(1.0f, 1.0f, 1.0f)
{
}

Color::Color(float r, float g, float b) : r(r), g(g), b(b)
{
}

SDL_Color
Color::to_sdl() const
{
  return {
    static_cast<Uint8>(r * 255),
    static_cast<Uint8>(g * 255),
    static_cast<Uint8>(b * 255)
  };
}

Color
Color::interpolate_progress(float frac)
{
  Color ret;

  float redm   = (frac > 0.5) ? 2.0f * (frac - 0.5f) : 0.0f;
  float greenm = (frac < 0.5) ? 2.0f * (0.5f - frac) : 0.0f;
  ret.r = 1.0f - redm;
  ret.g = 1.0f - greenm;
  ret.b = 0.0f;

  return ret;
}

}
