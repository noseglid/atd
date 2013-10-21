#ifndef __UI__PAUSEMENU_H__
#define __UI__PAUSEMENU_H__

#include "ui/Menu.h"

B_NS_UI

class PauseMenu : public Menu
{
  bool open;

  PauseMenu();
  PauseMenu(const PauseMenu& rhs);
  void operator=(const PauseMenu& rhs);

public:
  void _hide();
  void _show();
  void toggle();

  static PauseMenu& instance();
};

E_NS_UI

#endif
