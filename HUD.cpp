#include "HUD.h"
#include "Debug.h"
#include "Player.h"
#include "Text.h"
#include "GLTransform.h"
#include "ImageLoader.h"

#define HUD_BUTTON_WIDTH 80.0f
#define HUD_BUTTON_HEIGHT 64.0f

#define BAR_OFFSET 3.0f
#define BAR_PADDING 5.0f
#define BAR_HEIGHT (HUD_BUTTON_HEIGHT + 2 * BAR_PADDING)

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

  nbuttonsat[BOTTOM_LEFT] = 0;
  nbuttonsat[BOTTOM_RIGHT] = 0;
}

int
HUD::button_index(int x, int y) const
{
  y = screen_height - y;
  for (button_def def : button_definitions) {
    if (def.vertices.left  <= x &&
        def.vertices.right >= x &&
        def.vertices.bot   <= y &&
        def.vertices.top   >= y) {
      return def.button_index;
    }
  }

  return -1;
}

void
HUD::draw_banner(float yoffset) const
{
  glBegin(GL_TRIANGLE_STRIP);
    glVertex2f(
      BAR_OFFSET,
      yoffset + BAR_HEIGHT
    );

    glVertex2f(
      BAR_OFFSET,
      yoffset
    );

    glVertex2f(
      HUD::screen_width - BAR_OFFSET,
      yoffset + BAR_HEIGHT
    );

    glVertex2f(
      HUD::screen_width - BAR_OFFSET,
      yoffset
    );
  glEnd();
}

void
HUD::draw_button(button_def def) const
{
  glBindTexture(GL_TEXTURE_2D, def.texture);

  glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(def.vertices.left, def.vertices.top);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(def.vertices.left, def.vertices.bot);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(def.vertices.right, def.vertices.top);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(def.vertices.right, def.vertices.bot);
  glEnd();

  if (def.button_index == marked_button) {
    /* This button is currently 'marked' */

    glPointSize(12.0f);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(def.vertices.right, def.vertices.top);
    glEnd();
  }
}

void
HUD::draw_stats() const
{
  Text::set_color(0.0f, 0.80f, 0.0f);

  std::stringstream gold;
  gold << "Gold: " << Player::instance().gold;
  Text::overlay(gold.str(), 10, 40, false);

  std::stringstream lives;
  lives << "Lives: " << Player::instance().lives;
  Text::overlay(lives.str(), 10, 60, false);
}

void
HUD::tick() const
{
  GLTransform::enable2D();

  glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
  glDisable(GL_TEXTURE_2D);
  draw_banner(screen_height - (BAR_HEIGHT + BAR_OFFSET));
  draw_banner(BAR_OFFSET);

  for (button_def def : button_definitions) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_button(def);
  }

  int title_width, title_height;
  Text::set_color(0.853, 0.853, 0.853);
  Text::size(title, &title_width, &title_height);
  Text::overlay(
    title,
    screen_width / 2 - title_width / 2,
    BAR_OFFSET + BAR_HEIGHT / 2 + title_height / 2,
    false
  );

  draw_stats();

  GLTransform::disable2D();
}

void
HUD::mousedown(const GameEvent& ge) const
{
  if (!in_turf(ge.ev.button.x, ge.ev.button.y)) return;

  int bindex = button_index(ge.ev.button.x, ge.ev.button.y);
  if (0 > bindex) return;

  button_definitions.at(bindex).cb(bindex);
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
    y > (screen_height - BAR_OFFSET - BAR_HEIGHT) &&
    y < (screen_height - BAR_OFFSET);
}

void
HUD::calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const
{
  int i = abs(xindex) - 1;

  *left = (i + 1) * BAR_PADDING +
          i * (HUD_BUTTON_WIDTH + BAR_PADDING) + BAR_OFFSET;

  *right = (i + 1) * (HUD_BUTTON_WIDTH + 2 * BAR_PADDING) + BAR_OFFSET;

  if (0 > xindex) {
    int tmp = *left;
    *left = screen_width - *right;
    *right = screen_width - tmp;
  }

  *top = BAR_OFFSET + HUD_BUTTON_HEIGHT + BAR_PADDING;
  *bot = BAR_OFFSET + BAR_PADDING;
}

int
HUD::add_button(GLuint texture, button_cb cb, BUTTON_LOCATION loc)
{
  button_def def;
  def.texture = texture;
  def.cb = cb;
  def.button_index = button_definitions.size();
  def.loc = loc;
  calc_button_vertices(
    ((loc == BOTTOM_RIGHT) ? -1 : 1) * (++nbuttonsat[loc]),
    &def.vertices.left,
    &def.vertices.right,
    &def.vertices.top,
    &def.vertices.bot
  );

  button_definitions.push_back(def);
  return def.button_index;
}

void
HUD::remove_button(int idx)
{
  auto it = std::find_if(button_definitions.begin(), button_definitions.end(),
    [idx](button_def def) { return def.button_index == idx; });
  if (it == button_definitions.end()) {
    DBGWRN("No button index: " << idx << " defined in HUD.");
    return;
  }

  nbuttonsat[it->loc]--;
  button_definitions.erase(it);
}

void
HUD::mark_button(int bindex)
{
  marked_button = bindex;
}

void
HUD::set_title(std::string title)
{
  this->title = title;
}
