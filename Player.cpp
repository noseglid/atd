#include "Player.h"
#include "Game.h"
#include "text/Stream.h"

#include <sstream>

float Player::sell_factor = 0.5f;

Player::Player() : gold(0), lives(0), max_lives(0)
{
  stats_text();
}

void
Player::stats_text()
{
  float frac = (0 == max_lives) ? 1.0f : (float)lives / (float)max_lives;
  utils::Color lives_color = utils::Color::interpolate_progress(frac);

  text::Stream ts;
  ts << text::Stream::size(24.0f)
     << utils::colors::white << "Gold: " << utils::colors::gold << gold << "\n"
     << utils::colors::white << "Lives: " << lives_color << lives;
  hud::InfoBar::instance().set_info_text1(ts);
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
  max_lives = std::max(max_lives, lives);

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

bool
Player::is_alive()
{
  return (lives > 0);
}
