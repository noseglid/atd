#ifndef __HUD__INFOBOX_H__
#define __HUD__INFOBOX_H__

#include "hud/HUD.h"
#include "Game.h"

#include <string>
#include <list>

BEGIN_NS_HUD

class InfoBox
{
  struct boxdef {
    std::vector<std::string> lines;
    int x, y;
    int width, height, line_height;
    int snap;
  };

  typedef std::list<boxdef> boxlist_t;
  boxlist_t boxlist;
  int mousex, mousey;

  InfoBox();

  void tick() const;
  void mousemotion(const GameEvent& ge);
public:
  typedef boxlist_t::iterator id_t;

  static InfoBox& instance();

  /**
   * @param snap   Where to snap the box to, 0 for lower left, 1 for upper left,
   *               2 for upper right, 3 for lower right, only valid if follow_mouse = true.
   */
  id_t add_box(std::string text, bool follow_mouse = false, int snap = 1);
  void remove_box(id_t& id);
  id_t noid();
};

END_NS_HUD

#endif
