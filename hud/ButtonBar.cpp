#include "ButtonBar.h"
#include "GLTransform.h"

BEGIN_NS_HUD

ButtonBar&
ButtonBar::instance()
{
  static ButtonBar instance;
  return instance;
}

ButtonBar::ButtonBar()
{
  Game::instance().on("tick_nodepth", std::bind(&ButtonBar::tick, this));
  Game::instance().on("mousedown", std::bind(&ButtonBar::mousedown, this, std::placeholders::_1));
}

void
ButtonBar::tick() const
{
  GLTransform::enable2D();

  glColor4f(BAR_COLOR);
  glDisable(GL_TEXTURE_2D);
  draw_banner(BAR_OFFSET);

  for (button_def def : button_definitions) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_button(def);
  }

  GLTransform::disable2D();
}

void
ButtonBar::mousedown(const GameEvent& ge) const
{
  if (!in_turf(ge.ev.button.x, ge.ev.button.y)) return;

  int bindex = button_index(ge.ev.button.x, ge.ev.button.y);
  if (0 > bindex) return;

  button_definitions.at(bindex).cb(bindex);
}

END_NS_HUD
