#ifndef __HUD__TOPBAR_H__
#define __HUD__TOPBAR_H__

#include "Bar.h"

#include <vector>

BEGIN_NS_HUD

class InfoBar : public Bar
{
  std::string title;
  std::vector<std::string> info_text;

  InfoBar();
  void draw_stats() const;
  void tick() const;

public:
  static InfoBar& instance();

  void set_title(std::string title);
  void set_info_text(std::string text);
};

END_NS_HUD

#endif
