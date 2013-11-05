#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Exception.h"
#include "Purchasable.h"
#include "hud/InfoBar.h"

#include <de.h>

class Player : public de::Emitter<>
{
  friend class HUD::InfoBar;

  int gold;
  int lives, max_lives;

  Player(const Player&);
  void operator=(Player const&);

  void stats_text();

public:
  Player();

  static float sell_factor;

  Player *alter_gold(int delta);
  Player *alter_lives(int delta);

  bool purchase(Purchasable *p);
  int sell(Purchasable *p);
};

#endif
