#include "Player.h"
#include "Game.h"
#include "Text.h"

#include <sstream>

Player::Player() : gold(0), lives(0)
{
}

Player&
Player::instance()
{
  static Player instance;
  return instance;
}

void
Player::alter_gold(int delta)
{
  gold += delta;
}

void Player::alter_lives(int delta)
{
  lives +=delta;
  if (lives <= 0)
    throw PlayerDeath();
}

bool
Player::purchase(Purchasable *p)
{
  if (p->price > this->gold)
    return false;

  alter_gold(-p->price);
  return true;
}

int
Player::sell(Purchasable *p)
{
  int return_value = floor(p->price * 0.5f);
  alter_gold(return_value);
  return return_value;
}
