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
SDL_Color font_color;

#define SIZE_FACTOR(x) ((x) / TEXT_OVERLAY_PTSIZE)

Text::Text()
{
  Game::instance().on("tick_nodepth", std::bind(&Text::tick, this));
  font_color = { 255, 255, 255 };
}

void
Text::init(const int& screen_width, const int& screen_height)
{
  if (!(Text::font_world = TTF_OpenFont("fonts/Riky-Vampdator.ttf",
                                        TEXT_SCROLLING_PTSIZE))) {
    throw Exception("Could not load font file.");
  }

  if (!(Text::font_overlay = TTF_OpenFont("fonts/Riky-Vampdator.ttf",
                                          TEXT_OVERLAY_PTSIZE))) {
    throw Exception("Could not load font file.");
  }

  TTF_SetFontKerning(Text::font_overlay, 1);

  Text::screen_width  = screen_width;
  Text::screen_height = screen_height;
}

void
Text::size(const std::string& text, int *width, int *height, const float& ptsize)
{
  TTF_SizeText(font_overlay, text.c_str(), width, height);
  if (NULL != width)
    *width  *= SIZE_FACTOR(ptsize);
  if (NULL != height)
    *height *= SIZE_FACTOR(ptsize);
}

int
Text::overlay_line_skip(float ptsize)
{
  return TTF_FontLineSkip(font_overlay) * SIZE_FACTOR(ptsize);
}

void
Text::set_color(float r, float g, float b)
{
  font_color.r = r * 255;
  font_color.g = g * 255;
  font_color.b = b * 255;
}

GLuint
Text::create_texture(const std::string& text, TTF_Font *font, int& w, int& h)
{
  if (text.empty()) {
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  SDL_Surface *stext = TTF_RenderUTF8_Blended(font, text.c_str(), font_color);
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
    glColor3f(wt.color.r / 255.0f, wt.color.g / 255.0f, wt.color.b / 255.0f);
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
Text::overlay(const std::string& text, const int& x, const int& y, const float& ptsize, bool offbottom, bool offleft)
{
  GLTransform::enable2D();
  glEnable(GL_TEXTURE_2D);

  if (ptsize > TEXT_OVERLAY_PTSIZE) {
    DBGWRN("Requested overlay pt size is large than loaded. "
           "Will upscale which might look horrible. "
           "Consider increasing loaded size.");
  }

  int w, h;
  GLuint texture = Text::create_texture(text, font_overlay, w, h);
  if (0 == texture) {
    DBGWRN("Could not create texture from text: " << text);
    return;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glColor3f(1.0f, 1.0f, 1.0f);

  h *= SIZE_FACTOR(ptsize);
  w *= SIZE_FACTOR(ptsize);

  float yoffset = (offbottom) ? 2 * y : screen_height;
  float xoffset = (offleft)   ? 2 * x : screen_width;

  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(xoffset - x,     yoffset - y);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(xoffset - x,     yoffset - y - h);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(xoffset - x + w, yoffset - y);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(xoffset - x + w, yoffset - y - h);
  glEnd();

  glDeleteTextures(1, &texture);

  GLTransform::disable2D();
}

void
Text::scrolling(const std::string& text, const Vector3& pos)
{
  WorldText wt;
  wt.texture = Text::create_texture(text, font_world, wt.width, wt.height);
  if (0 == wt.texture) {
    DBGWRN("Could not create scrolling texture from text: " << text);
    return;
  }

  wt.pos   = pos;
  wt.color = font_color;

  scrollings.push_back(wt);
}
