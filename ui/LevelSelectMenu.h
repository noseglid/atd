#ifndef __UI__LEVELSELECTMENU_H__
#define __UI__LEVELSELECTMENU_H__

#include "ui/Menu.h"

B_NS_UI

class LevelSelectMenu : public Menu
{
  LevelSelectMenu();

public:
  static LevelSelectMenu& instance();
};

E_NS_UI

#endif
