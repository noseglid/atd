#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Exception.h"
#include "Purchasable.h"
#include "hud/InfoBar.h"

class PlayerDeath : public Exception
{
public:
  PlayerDeath() : Exception("Player died") {}
};

class Player
{
  friend class HUD::InfoBar;

  int gold;
  int lives;

  Player();
  void operator=(Player const&);
  Player(const Player&);
  void stats_text();

  void tick() const;

public:
  static float sell_factor;

  static Player& instance();

  void alter_gold(int delta);
  void alter_lives(int delta);

  bool purchase(Purchasable *p);
  int sell(Purchasable *p);
};

#endif
