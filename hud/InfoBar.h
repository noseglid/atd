#ifndef __HUD__TOPBAR_H__
#define __HUD__TOPBAR_H__

#include "Bar.h"

#include <vector>
#include <string>

B_NS_HUD

class InfoBar : public Bar
{
  std::string title;
  std::vector<std::string> info_text1, info_text2;

  InfoBar();
  void draw_stats() const;
  void tick() const;

  void set_info_text(std::vector<std::string>& vector, std::string text);
  void draw_info_text(bool leftjustify, const std::vector<std::string>& text) const;

public:
  static InfoBar& instance();

  void set_title(std::string title);
  void set_info_text1(std::string text);
  void set_info_text2(std::string text);
};

E_NS_HUD

#endif
