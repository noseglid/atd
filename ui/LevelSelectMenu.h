#ifndef __UI__LEVELSELECTMENU_H__
#define __UI__LEVELSELECTMENU_H__

#include "ui/Menu.h"

namespace ui {

class LevelSelectMenu : public Menu
{
  LevelSelectMenu();

public:
  static LevelSelectMenu& instance();
};

}

#endif
