#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Exception.h"
#include "Purchasable.h"

class PlayerDeath : public Exception
{
public:
  PlayerDeath() : Exception("Player died") {}
};

class Player
{
  friend class HUD;

  int gold;
  int lives;

  Player();
  void operator=(Player const&);
  Player(const Player&);

public:
  static Player& instance();

  void alter_gold(int delta);
  void alter_lives(int delta);

  bool purchase(Purchasable *p);
};

#endif
