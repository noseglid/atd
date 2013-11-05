#ifndef __HUD__INFOBOX_H__
#define __HUD__INFOBOX_H__

#include "hud/HUD.h"
#include "engine/Engine.h"
#include "text/Stream.h"

#include <de.h>

B_NS_HUD

#define BOX_PADDING 3.0f
#define BOX_MARGIN 3.0f

class InfoBox
{
public:
  enum SNAP {
    SNAP_BOTLEFT,
    SNAP_TOPLEFT,
    SNAP_TOPRIGHT,
    SNAP_BOTRIGHT,
    SNAP_CENTER
  };

private:

  int top, left, width, height;
  text::Stream stream;
  SNAP snap;
  bool followmouse;
  de::Emitter<engine::Event>::id_t mousemotion_emit;

  void set_bounding_box();

public:

  /**
   * Creates a new infobox.
   *
   * @param snap          Where the box should be placed. If 'followmouse' is true,
   *                      then this parameter decides where the mouse should snap to.
   *
   * @param followmouse   If the box should follow the mouse or not.
   */
  InfoBox(text::Stream stream, SNAP snap = SNAP_BOTLEFT, bool followmouse = false);
  ~InfoBox();

  void set_content(text::Stream stream);

  void clear();
  void draw();
};

E_NS_HUD

#endif
