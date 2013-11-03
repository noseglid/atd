#ifndef __TEXT_H__
#define __TEXT_H__

#include "math/Math.h"
#include "utils/Color.h"

#include <string>
#include <vector>

#include <OpenGL/gl.h>
#include <SDL/SDL_ttf.h>

#define B_NS_TEXT namespace text {
#define E_NS_TEXT }

B_NS_TEXT

class Text
{
  Text();
  Text& operator=(const Text& rhs);
  Text(const Text&);

public:
 TTF_Font *font_world;
 TTF_Font *font_overlay;

  static const int OVERLAY_PTSIZE   = 48;
  static const int SCROLLING_PTSIZE = 32;

  static Text& instance();

  static void size(
    const std::string& text,
    int *width = NULL,
    int *height = NULL,
    const float& ptsize = 16.0f,
    TTF_Font *font = NULL
  );

  static GLuint create_texture(
    const std::string& text,
    TTF_Font *font,
    utils::Color clr,
    int& w,
    int& h
  );

  static float size_factor(float size);
  static int line_skip(float ptsize, TTF_Font *font = NULL);
  static std::string linebreak(std::string s, int col = 40);
};

E_NS_TEXT

#endif
