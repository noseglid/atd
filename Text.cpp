#include "Text.h"
#include "Game.h"
#include "Debug.h"
#include "GLTransform.h"

#include <OpenGL/glu.h>
#include <SDL/SDL.h>

TTF_Font *Text::font_world   = NULL;
TTF_Font *Text::font_overlay = NULL;
int Text::screen_width  = 0;
int Text::screen_height = 0;
Text Text::instance = Text();
std::vector<WorldText> Text::scrollings = std::vector<WorldText>();

Text::Text()
{
  Game::instance().on("tick_nodepth", std::bind(&Text::tick, this));
}

void
Text::init(const int& screen_width, const int& screen_height)
{
  if (!(Text::font_world = TTF_OpenFont("/Library/Fonts/Arial.ttf", 128))) {
    throw Exception("Could not load font file.");
  }

  if (!(Text::font_overlay = TTF_OpenFont("/Library/Fonts/Arial.ttf", 16))) {
    throw Exception("Could not load font file.");
  }

  TTF_SetFontStyle(Text::font_overlay, TTF_STYLE_BOLD);

  Text::screen_width  = screen_width;
  Text::screen_height = screen_height;
}

GLuint
Text::create_texture(const std::string& text, TTF_Font *font, int& w, int& h)
{
  glEnable(GL_BLEND);
  SDL_Color fg = { 255, 255, 255 };

  TTF_SizeText(font, text.c_str(), &w, &h);
  SDL_Surface *intermediary = SDL_CreateRGBSurface(0, w, h, 32,
      0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

  SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, text.c_str(), fg);
  SDL_BlitSurface(text_surface, 0, intermediary, 0);
  SDL_FreeSurface(text_surface);


  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, intermediary->w, intermediary->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels);

  SDL_FreeSurface(intermediary);

  return texture;
}

void
Text::tick()
{
  glDisable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);

  auto it = scrollings.begin();
  while (it != scrollings.end()) {
    WorldText& wt = *it;

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, wt.texture);
    glTranslatef(wt.pos.x + wt.delta.x, wt.pos.y + wt.delta.y, wt.pos.z + wt.delta.z);
    GLTransform::billboard();
    glColor3f(wt.color.x, wt.color.y, wt.color.z);
    float height = 0.25f;
    float width = (float)wt.width/(float)wt.height * height;
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.1f + height, 0.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.1f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.1f + height, 0.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(width, 0.1f, 0.0f);
    glEnd();
    wt.delta.y += 0.005;

    it = (wt.delta.length() >= 0.5f) ? scrollings.erase(it) : it + 1;
    glPopMatrix();
  }

  glEnable(GL_LIGHTING);
}

void
Text::overlay(const std::string& text, const int& x, const int& y, bool offbottom, bool offleft)
{
  GLTransform::enable2D();
  glEnable(GL_TEXTURE_2D);

  int w, h;
  GLuint texture = Text::create_texture(text, font_overlay, w, h);
  glBindTexture(GL_TEXTURE_2D, texture);
  glColor3f(1.0f, 1.0f, 1.0f);

  float yoffset = (offbottom) ? 2 * y : screen_height;
  float xoffset = (offleft)   ? 2 * x : screen_width;
  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xoffset - x,     yoffset - y + h);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xoffset - x,     yoffset - y);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xoffset - x + w, yoffset - y + h);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xoffset - x + w, yoffset - y);
  glEnd();
  glDeleteTextures(1, &texture);

  GLTransform::disable2D();
}

void
Text::scrolling(const std::string& text, const Vector3& pos, Vector3 color)
{
  WorldText wt;
  wt.texture = Text::create_texture(text, font_world, wt.width, wt.height);
  wt.pos     = pos;
  wt.color   = color;

  scrollings.push_back(wt);
}
