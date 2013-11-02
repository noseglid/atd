#include "Player.h"
#include "Game.h"
#include "Text.h"

#include <sstream>

float Player::sell_factor = 0.5f;

Player::Player() : gold(0), lives(0)
{
  stats_text();
}

void
Player::stats_text()
{
  std::stringstream ss;
  ss << "Gold: " << gold << std::endl
     << "Lives: " << lives;
  HUD::InfoBar::instance().set_info_text1(ss.str());
}

Player *
Player::alter_gold(int delta)
{
  gold += delta;
  stats_text();

  return this;
}

Player *
Player::alter_lives(int delta)
{
  if (0 < lives && 0 >= lives + delta) {
    /* We are currently alive, and this alteration will kill us */
    emit("death");
  }

  lives = std::max(lives + delta, 0);

  stats_text();

  return this;
}

bool
Player::purchase(Purchasable *p)
{
  if (p->price > this->gold)
    return false;

  alter_gold(-p->price);
  stats_text();
  return true;
}

int
Player::sell(Purchasable *p)
{
  int return_value = floor(p->price * Player::sell_factor);
  alter_gold(return_value);
  stats_text();
  return return_value;
}
