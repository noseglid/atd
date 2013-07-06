#ifndef __TEXT_H__
#define __TEXT_H__

#include "math/Math.h"

#include <string>
#include <vector>

#include <OpenGL/gl.h>
#include <SDL/SDL_ttf.h>

#define TEXT_OVERLAY_PTSIZE 48
#define TEXT_SCROLLING_PTSIZE 32

struct WorldText
{
  GLuint texture;
  Vector3 pos, delta;
  SDL_Color color;
  int width, height;
};

class Text
{
  static Text instance;
  static TTF_Font *font_world, *font_overlay;
  static int screen_width, screen_height;
  static std::vector<WorldText> scrollings;

  Text();
  Text& operator=(const Text& rhs);
  void tick();
  static GLuint create_texture(const std::string& text, TTF_Font *font, int& w, int& h);

public:

  static void init(const int& screen_width, const int& screen_height);
  static void size(const std::string& text, int *width = NULL, int *height = NULL, const float& ptsize = 16.0f);
  static int overlay_line_skip(float ptsize);

  static void set_color(float r, float g, float b);
  static void overlay(const std::string& text, const int& x, const int& y,
                      const float& ptsize = 16.0, bool offbottom = true, bool offleft = true);
  static void scrolling(const std::string& text, const Vector3& pos);

  static std::string linebreak(std::string s, int col = 40);

};

#endif
