#ifndef __UI__PAUSEMENU_H__
#define __UI__PAUSEMENU_H__

#include "ui/Menu.h"

namespace ui {

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

}

#endif
