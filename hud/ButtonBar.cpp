#include "hud/ButtonBar.h"
#include "gl/Transform.h"

B_NS_HUD

ButtonBar&
ButtonBar::instance()
{
  static ButtonBar instance;
  return instance;
}

ButtonBar::ButtonBar()
{
  nbuttonsat[Button::LOCATION_LEFT]  = 0;
  nbuttonsat[Button::LOCATION_RIGHT] = 0;

  engine::Engine::instance().on("tick_nodepth", std::bind(&ButtonBar::tick, this));
}

void
ButtonBar::tick() const
{
  if (!visible) return;

  gl::Transform::enable2D();

  glColor4f(BAR_COLOR);
  glDisable(GL_TEXTURE_2D);
  draw_banner(BAR_OFFSET);

  for (Button *b : buttons) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    b->draw();
    glPopMatrix();
  }

  gl::Transform::disable2D();
}

int
ButtonBar::add_button(Button *button, Button::LOCATION loc)
{
  buttons.push_back(button);
  int index = (loc == Button::LOCATION_LEFT ? 1 : -1) * (++nbuttonsat[loc]);
  button->set_index(index);
  return index;
}

void
ButtonBar::remove_button(Button *button)
{
  nbuttonsat[button->get_location()]--;
  buttons.erase(std::remove(buttons.begin(), buttons.end(), button), buttons.end());
  delete button;
}

void
ButtonBar::clear_buttons()
{
  for (auto it = buttons.begin(); it != buttons.end();) {
    delete *(it++);
  }
  buttons.clear();

  nbuttonsat[Button::LOCATION_LEFT] = 0;
  nbuttonsat[Button::LOCATION_RIGHT] = 0;
}

void
ButtonBar::unmark_all()
{
  for (Button *b : buttons) {
    b->mark(false);
  }
}

E_NS_HUD
