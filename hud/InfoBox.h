#ifndef __HUD__INFOBOX_H__
#define __HUD__INFOBOX_H__

#include "hud/HUD.h"
#include "Game.h"

#include <string>
#include <vector>

BEGIN_NS_HUD

class InfoBox
{
  struct boxdef {
    std::vector<std::string> lines;
    int x, y;
    int width, height, line_height;
  };

  typedef std::vector<boxdef> boxlist_t;
  boxlist_t boxlist;
  int mousex, mousey;

  InfoBox();

  void tick() const;
  void mousemotion(const GameEvent& ge);
public:
  typedef boxlist_t::iterator boxid_t;

  static InfoBox& instance();

  boxid_t add_box(std::string text, bool follow_mouse = false);
  void remove_box(boxid_t id);
};

END_NS_HUD

#endif
