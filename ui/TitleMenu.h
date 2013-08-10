#ifndef __UI__TITLEMENU_H__
#define __UI__TITLEMENU_H__

#include "engine/Engine.h"
#include "ui/Menu.h"

#include <de.h>

B_NS_UI

class TitleMenu : public Menu
{
  TitleMenu();
  TitleMenu(const TitleMenu& rhs);
  void operator=(const TitleMenu& rhs);
  void tick(const engine::Event& ev);

public:
  static TitleMenu& instance();
};

E_NS_UI

#endif
