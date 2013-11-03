#include "Text.h"
#include "engine/Engine.h"
#include "engine/Video.h"
#include "Exception.h"
#include "Debug.h"

#include <OpenGL/glu.h>
#include <SDL/SDL.h>

B_NS_TEXT

Text::Text() : font_world(NULL), font_overlay(NULL)
{
  if (!(font_world = TTF_OpenFont("resources/fonts/Riky-Vampdator.ttf",
                                  Text::SCROLLING_PTSIZE))) {
    throw Exception("Could not load font file.");
  }

  if (!(font_overlay = TTF_OpenFont("resources/fonts/Riky-Vampdator.ttf",
                                    Text::OVERLAY_PTSIZE))) {
    throw Exception("Could not load font file.");
  }

  TTF_SetFontKerning(font_overlay, 1);
}

Text&
Text::instance()
{
  static Text instance;
  return instance;
}

void
Text::size(const std::string& text, int *width, int *height, const float& ptsize, TTF_Font *font)
{
  if (NULL == font) font = Text::instance().font_overlay;

  TTF_SizeText(font, text.c_str(), width, height);
  if (NULL != width)
    *width  *= Text::size_factor(ptsize);
  if (NULL != height)
    *height *= Text::size_factor(ptsize);
}

float
Text::size_factor(float size)
{
  return size / OVERLAY_PTSIZE;
}

int
Text::line_skip(float ptsize, TTF_Font *font)
{
  if (NULL == font) font = Text::instance().font_overlay;

  return TTF_FontLineSkip(font) * Text::size_factor(ptsize);
}

GLuint
Text::create_texture(
  const std::string& text,
  TTF_Font *font,
  utils::Color clr,
  int& w,
  int& h
)
{
  if (text.empty()) {
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  SDL_Surface *stext = TTF_RenderUTF8_Blended(font, text.c_str(), clr.to_sdl());
  if (NULL == stext) {
    DBGERR("Could not create blended text, msg: " << text);
    return 0;
  }

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  w = stext->w;
  h = stext->h;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stext->w, stext->h, 0,
               GL_BGRA, GL_UNSIGNED_BYTE, stext->pixels);

  SDL_FreeSurface(stext);

  return texture;
}

std::string
Text::linebreak(std::string s, int col)
{
  int i = 0;
  std::string space = " ";
  while ((i += col) < s.size()) {
    auto it = std::find_end(s.begin(), s.begin() + i, space.begin(), space.end());
    if (it == s.end()) {
      /* No space found, can't line break */
      return s;
    }

    s.replace(it, it + 1, 1, '\n');
  }

  return s;
}

E_NS_TEXT
