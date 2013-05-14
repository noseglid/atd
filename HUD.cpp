#include "HUD.h"
#include "Debug.h"
#include "Player.h"
#include "Text.h"
#include "GLTransform.h"
#include "ImageLoader.h"

#define HUD_BUTTON_WIDTH 80.0f
#define HUD_BUTTON_HEIGHT 64.0f

#define BOTTOM_BAR_OFFSET 3.0f
#define BOTTOM_BAR_PADDING 5.0f
#define BOTTOM_BAR_HEIGHT (HUD_BUTTON_HEIGHT + 2 * BOTTOM_BAR_PADDING)

int HUD::screen_width  = 0;
int HUD::screen_height = 0;

HUD&
HUD::instance()
{
  static HUD instance;
  return instance;
}

HUD::HUD() : marked_button(-1)
{
  Game::instance().on("tick_nodepth", std::bind(&HUD::tick, this));
  Game::instance().on("mousedown", std::bind(&HUD::mousedown, this, std::placeholders::_1));
}

int
HUD::button_index(int x, int y) const
{
  x -= (BOTTOM_BAR_OFFSET + BOTTOM_BAR_PADDING);
  int i = -1;
  while (x > 0) {
    x -= (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING);
    ++i;
  }
  return i;
}

void
HUD::draw_bottom_banner() const
{
  glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(
      BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + BOTTOM_BAR_HEIGHT
    );

    glVertex2f(
      BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET
    );

    glVertex2f(
      HUD::screen_width - BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + BOTTOM_BAR_HEIGHT
    );

    glVertex2f(
      HUD::screen_width - BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET
    );
  glEnd();
}

void
HUD::draw_button(GLuint texture, int i, bool marked) const
{
  glBindTexture(GL_TEXTURE_2D, texture);

  glBegin(GL_TRIANGLE_STRIP);
    /* Top Left */
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(
      (i + 1) * BOTTOM_BAR_PADDING + i * (HUD_BUTTON_WIDTH + BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + HUD_BUTTON_HEIGHT + BOTTOM_BAR_PADDING
    );

    /* Bottom Left */
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(
      (i + 1) * BOTTOM_BAR_PADDING + i * (HUD_BUTTON_WIDTH + BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + BOTTOM_BAR_PADDING
    );

    /* Top Right */
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(
      (i + 1) * (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + HUD_BUTTON_HEIGHT + BOTTOM_BAR_PADDING
    );

    /* Bottom Right */
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(
      (i + 1) * (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + BOTTOM_BAR_PADDING
    );
  glEnd();

  if (marked) {
    glPointSize(12.0f);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(
      (i + 1) * (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
      BOTTOM_BAR_OFFSET + HUD_BUTTON_HEIGHT + BOTTOM_BAR_PADDING
    );
    glEnd();
  }
}

void
HUD::draw_stats() const
{
  std::stringstream gold;
  gold << "Gold: " << Player::instance().gold;
  Text::overlay(gold.str(), 10, 30, false);

  std::stringstream lives;
  lives << "Lives: " << Player::instance().lives;
  Text::overlay(lives.str(), 10, 50, false);
}

void
HUD::tick() const
{
  GLTransform::enable2D();

  glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
  glDisable(GL_TEXTURE_2D);
  draw_bottom_banner();

  for (button_def def : button_definitions) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_button(def.texture, def.button_index, def.button_index == marked_button);
  }

  draw_stats();

  GLTransform::disable2D();
}

void
HUD::mousedown(const GameEvent& ge) const
{
  if (!in_turf(ge.ev.button.x, ge.ev.button.y)) return;

  int index = button_index(ge.ev.button.x, ge.ev.button.y);
  if (0 > index || button_definitions.size() <= index) return;

  button_definitions[index].cb(index);
}

void
HUD::init(int screen_width, int screen_height)
{
  HUD::screen_width  = screen_width;
  HUD::screen_height = screen_height;
  HUD::instance();
}

bool
HUD::in_turf(int x, int y) const
{
  return
    y > (screen_height - BOTTOM_BAR_OFFSET - BOTTOM_BAR_HEIGHT) &&
    y < (screen_height - BOTTOM_BAR_OFFSET);
}

int
HUD::add_button(GLuint texture, button_cb cb)
{
  button_def def;
  def.texture = texture;
  def.cb = cb;
  def.button_index = button_definitions.size();

  button_definitions.push_back(def);
  return def.button_index;
}


void
HUD::mark_button(int index)
{
  marked_button = index;
}
