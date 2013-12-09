#ifndef __UI__SETUSERMENU_H__
#define __UI__SETUSERMENU_H__

#include "engine/Engine.h"
#include "ui/Menu.h"

#include <de.h>

namespace ui {

class SetUserMenu : public Menu
{
  SetUserMenu();
  SetUserMenu(const SetUserMenu& rhs);
  void operator=(const SetUserMenu& rhs);

  void keyup(const engine::Event& ev);

public:
  static SetUserMenu& instance();

  void evolve();
};

}

#endif
