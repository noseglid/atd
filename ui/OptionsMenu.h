#ifndef __UI__OPTIONSMENU_H__
#define __UI__OPTIONSMENU_H__

#include "ui/Menu.h"

B_NS_UI

class OptionsMenu : public Menu
{
  Menu *back;

  OptionsMenu();
  OptionsMenu(const OptionsMenu& rhs);
  void operator=(const OptionsMenu& rhs);

public:
  static OptionsMenu& instance();
  Menu *get_back() const;
};

E_NS_UI

#endif
