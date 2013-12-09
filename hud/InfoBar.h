#ifndef __HUD__TOPBAR_H__
#define __HUD__TOPBAR_H__

#include "Bar.h"
#include "text/Stream.h"

#include <vector>
#include <string>

namespace hud {

class InfoBar : public Bar
{
  std::string title;
  text::Stream info_text1, info_text2;

  InfoBar();
  void draw_stats() const;
  void tick() const;

public:
  static InfoBar& instance();

  void set_title(std::string title);
  void set_info_text1(text::Stream stream);
  void set_info_text2(text::Stream stream);
};

}

#endif
