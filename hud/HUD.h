#ifndef __HUD__HUD_H__
#define __HUD__HUD_H__

#define BEGIN_NS_HUD namespace HUD {
#define END_NS_HUD }

BEGIN_NS_HUD

class HUD
{
  friend class Bar;
  friend class InfoBox;
  friend class InfoBar;
  friend class ButtonBar;

  static int screen_width, screen_height;

public:
  static void init(int screen_width, int screen_height);
};


END_NS_HUD

#endif
