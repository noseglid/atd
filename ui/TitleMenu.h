#ifndef __UI__TITLEMENU_H__
#define __UI__TITLEMENU_H__

#include "engine/Engine.h"
#include "ui/Menu.h"

#include <de.h>

namespace ui {

class TitleMenu : public Menu
{
  TitleMenu();
  TitleMenu(const TitleMenu& rhs);
  void operator=(const TitleMenu& rhs);
  void tick(const engine::Event& ev);

public:
  void display(bool visible = true);

  static TitleMenu& instance();
};

}

#endif
