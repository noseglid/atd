#ifndef __HUD__BOTTOMBAR_H__
#define __HUD__BOTTOMBAR_H__

#include "engine/Engine.h"
#include "hud/Bar.h"
#include "hud/Button.h"

#include <vector>
#include <de.h>

B_NS_HUD

class ButtonBar : public Bar, public de::Emitter<int>
{
  int nbuttonsat[Button::LOCATION_END];

  std::vector<Button *> buttons;

  ButtonBar();

  void tick() const;

public:
  static ButtonBar& instance();

  int add_button(Button *button, Button::LOCATION loc = Button::LOCATION_LEFT);
  void remove_button(Button *button);
  void unmark_all();
};


E_NS_HUD

#endif
