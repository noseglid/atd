#ifndef __HUD__BOTTOMBAR_H__
#define __HUD__BOTTOMBAR_H__

#include "Bar.h"
#include "Game.h"

BEGIN_NS_HUD

class ButtonBar : public Bar
{
  void mousedown(const GameEvent& ev) const;

  ButtonBar();

  void tick() const;

public:
  static ButtonBar& instance();
  void calc_button_vertices(int xindex, float *left, float *right, float *top, float *bot) const;
};


END_NS_HUD

#endif
