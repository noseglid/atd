#ifndef __HUD__HUD_H__
#define __HUD__HUD_H__

#define B_NS_HUD namespace HUD {
#define E_NS_HUD }

B_NS_HUD

class HUD
{
  friend class Bar;
  friend class Button;
  friend class InfoBox;
  friend class InfoBar;
  friend class ButtonBar;

  static int screen_width, screen_height;

public:
  static void init(int screen_width, int screen_height);
};


E_NS_HUD

#endif
