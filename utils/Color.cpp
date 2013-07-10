#include "Color.h"

BEGIN_NS_UTILS

namespace colors {
  Color red       = Color(0.86f, 0.09f, 0.10f);
  Color green     = Color(0.00f, 1.00f, 0.00f);
  Color blue      = Color(0.00f, 0.00f, 1.00f);
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

END_NS_UTILS
