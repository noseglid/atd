#ifndef __UI__SETUSERMENU_H__
#define __UI__SETUSERMENU_H__

#include "engine/Engine.h"
#include "ui/Menu.h"

#include <de.h>

B_NS_UI

class SetUserMenu : public Menu
{
  SetUserMenu();
  SetUserMenu(const SetUserMenu& rhs);
  void operator=(const SetUserMenu& rhs);
  void tick(const engine::Event& ev);

public:
  static SetUserMenu& instance();

  std::string get_form_value(const char *id) const;
  void evolve();
};

E_NS_UI

#endif
