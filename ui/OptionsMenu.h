#ifndef __UI__OPTIONSMENU_H__
#define __UI__OPTIONSMENU_H__

#include "ui/Menu.h"

B_NS_UI

class OptionsMenu : public Menu
{
  OptionsMenu();
  OptionsMenu(const OptionsMenu& rhs);
  void operator=(const OptionsMenu& rhs);

public:
  static OptionsMenu& instance();
};

E_NS_UI

#endif
